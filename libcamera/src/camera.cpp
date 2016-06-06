
#include "camera.hpp"

extern "C" {
#include <fcntl.h>            // open
#include <linux/usb/video.h>  // define UVC_*
#include <linux/uvcvideo.h>   // UVC ExtUnit control
#include <sys/ioctl.h>        // ioctl
#include <sys/mman.h>         // mmap
#include <unistd.h>           // close
}
#include <algorithm>
#include <cstring>  // strerror
#include <fstream>  // std::ofstream
#include <iomanip>
#include <iostream>
#include <stdexcept>

Camera::Camera(int argc, char** argv)
    : _device("/dev/video0")
    , _width(640)
    , _height(480)
    , _fps(15)
    , _bufferNb(4)
    , _verbose(false)
    , _fd(-1) {
  if (cmdOptionExists(argc, argv, "-d")) {
    _device = getCmdOption(argc, argv, "-d");
  } else
    throw std::runtime_error("missing device file (e.g. /dev/video0)");

  if (cmdOptionExists(argc, argv, "-w")) {
    _width = std::atoi(getCmdOption(argc, argv, "-w").c_str());
  }

  if (cmdOptionExists(argc, argv, "-h")) {
    _height = std::atoi(getCmdOption(argc, argv, "-h").c_str());
  }

  if (cmdOptionExists(argc, argv, "-fps")) {
    _fps = std::atoi(getCmdOption(argc, argv, "-fps").c_str());
  }

  if (cmdOptionExists(argc, argv, "-nb")) {
    _bufferNb = std::atoi(getCmdOption(argc, argv, "-nb").c_str());
  }

  if (cmdOptionExists(argc, argv, "--verbose"))
    _verbose = true;

  if (cmdOptionExists(argc, argv, "--info")) {
    getInfo();
  }
}

Camera::~Camera() {
  if (_fd != -1) {
    stopStreaming();
    unInitMmapStreaming();
    closeDevice();
  }
}

extern "C" {
//! @brief Small helper to remove warning (old style cast)
static inline std::uint32_t
getYUYVformat() {
  return V4L2_PIX_FMT_YUYV;
}
//! @brief small helper to remove warning (old style cast)
static const void* mapFailed = MAP_FAILED;
}

void
Camera::getInfo() const {
  std::cout << "(INFO) device: " << _device << std::endl
            << "(INFO) resolution: " << _width << "x" << _height << std::endl
            << "(INFO) fps: " << _fps << std::endl
            << "(INFO) ring buffer size: " << _bufferNb << std::endl
            << "(INFO) verbose: " << _verbose << std::endl;
}

void
Camera::openDevice() {
  openDevice(_device);
}

void
Camera::openDevice(const std::string& deviceName) {
  if (_verbose) {
    std::cout << "(VERBOSE) " << deviceName << ": open device" << std::endl;
  }
  // fd = ::open(deviceName.c_str(), O_RDWR | O_CLOEXEC);
  _fd = ::open(deviceName.c_str(), O_RDWR | O_NONBLOCK | O_CLOEXEC);
  if (_fd < 0) {
    _fd = -1;
    throw std::runtime_error(deviceName + ": open fails: " +
                             std::strerror(errno));
  }
  _device = deviceName;
}

void
Camera::setResolution() {
  setResolution(_width, _height);
}

void
Camera::setResolution(uint32_t w, uint32_t h) {
  if (_verbose) {
    std::cout << "(VERBOSE) " << _device << ": set resolution to " << w << "x"
              << h << std::endl;
  }
  struct v4l2_format fmt;
  ::memset(&fmt, 0, sizeof(fmt));
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  // for a video device
  fmt.fmt.pix.field = V4L2_FIELD_NONE;     // not interlaced camera
  fmt.fmt.pix.width = w;
  fmt.fmt.pix.height = h;
  fmt.fmt.pix.pixelformat = getYUYVformat();
  fmt.fmt.pix.colorspace = V4L2_COLORSPACE_JPEG;
  if (-1 == ::ioctl(_fd, VIDIOC_S_FMT, &fmt)) {
    std::cerr << "(ERROR) " << _device << ": VIDIOC_S_FMT ("
              << std::to_string(_IOC_NR(VIDIOC_S_FMT))
              << "): " << std::strerror(errno) << std::endl;
    throw std::runtime_error(_device + ": VIDIOC_S_FMT: " +
                             std::strerror(errno));
  }
  _width = w;
  _height = h;
}

void
Camera::setFrameRate() {
  setFrameRate(_fps);
}

void
Camera::setFrameRate(std::uint32_t frame) {
  if (_verbose) {
    std::cout << "(VERBOSE) " << _device << ": set framerate to " << frame
              << std::endl;
  }
  struct v4l2_streamparm parm;
  ::memset(&parm, 0, sizeof(parm));
  parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  parm.parm.capture.timeperframe.numerator = 1;
  parm.parm.capture.timeperframe.denominator = frame;
  // indicates that the device can vary its frame rate
  parm.parm.capture.capability = V4L2_CAP_TIMEPERFRAME;
  parm.parm.capture.extendedmode = 0;
  if (-1 == ::ioctl(_fd, VIDIOC_S_PARM, &parm)) {
    std::cerr << "(ERROR) " << _device  << ": VIDIOC_S_PARM ("
              << std::to_string(_IOC_NR(VIDIOC_S_PARM))
              <<"): " << std::strerror(errno) << std::endl;
    throw std::runtime_error(_device + ": VIDIOC_S_PARM fails: " +
                             std::strerror(errno));
  }
  _fps = frame;
}

void
Camera::initMmapStreaming() {
  initMmapStreaming(_bufferNb);
}

void
Camera::initMmapStreaming(int bufferCount) {
  _bufferNb = bufferCount;
  if (_verbose) {
    std::cout << "(VERBOSE) " << _device << ": allocating memory (" << _bufferNb
              << " buffers)" << std::endl;
  }
  // First request a number of buffer.
  struct v4l2_requestbuffers req;
  ::memset(&req, 0, sizeof(req));
  req.count = _bufferNb;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;
  if (-1 == ::ioctl(_fd, VIDIOC_REQBUFS, &req)) {
    std::cerr << "(ERROR) " << _device << ": VIDIOC_REQBUFS ("
              << std::to_string(_IOC_NR(VIDIOC_REQBUFS))
              << "): " << std::strerror(errno) << std::endl;
    throw std::runtime_error(_device + ": VIDIOC_REQBUFS fails: " +
                             std::strerror(errno));
  }
  if (req.count != _bufferNb) {
    throw std::runtime_error(_device +
                             ": Insufficient memory to create "
                             "enough RAW buffer in the kernel "
                             "ring buffer.");
  }

  // Get pointer and size of each one
  for (auto i = 0; i < _bufferNb; ++i) {
    struct v4l2_buffer buf;
    ::memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;
    if (-1 == ::ioctl(_fd, VIDIOC_QUERYBUF, &buf)) {
    std::cerr << "(ERROR) " << _device << ": VIDIOC_QUERYBUF ("
              << std::to_string(_IOC_NR(VIDIOC_QUERYBUF))
              << "): " << std::strerror(errno) << std::endl;
      throw std::runtime_error(_device + ": VIDIOC_QUERYBUF fails: " +
                               std::to_string(i) + ": " + std::strerror(errno));
    }

    // MMAP them
    void* startAddr = ::mmap(NULL, buf.length, PROT_READ | PROT_WRITE,
                             MAP_SHARED, _fd, buf.m.offset);
    if (mapFailed == startAddr) {
      throw std::runtime_error(_device + ": Cannot mmap kernel buffer " +
                               std::to_string(i) + ": " + strerror(errno));
    }
    _v4l2Buffers.push_back(std::make_pair(startAddr, buf.length));

    // Enqueue them
    if (-1 == ::ioctl(_fd, VIDIOC_QBUF, &buf)) {
      throw std::runtime_error(_device + ": VIDIOC_QBUF fails: " +
                               std::to_string(i) + ": " + strerror(errno));
    }
  }  // end of For
}

void
Camera::startStreaming() {
  if (_verbose) {
    std::cout << "(VERBOSE) " << _device << ": start streaming" << std::endl;
  }
  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (-1 == ::ioctl(_fd, VIDIOC_STREAMON, &type)) {
    std::cerr << "(ERROR) " << _device << ": VIDIOC_STREAMON ("
              << std::to_string(_IOC_NR(VIDIOC_STREAMON))
              << "): " << std::strerror(errno) << std::endl;
    unInitMmapStreaming();
    throw std::runtime_error(_device + ": Cannot start streaming: " +
                             std::strerror(errno));
  }
}

void
Camera::stopStreaming() {
  if (_verbose) {
    std::cout << "(VERBOSE) " << _device << ": stop streaming" << std::endl;
  }
  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (-1 == ::ioctl(_fd, VIDIOC_STREAMOFF, &type)) {
    std::cerr << "(ERROR) " << _device << ": VIDIOC_STREAMOFF ("
              << std::to_string(_IOC_NR(VIDIOC_STREAMOFF))
              << "): " << std::strerror(errno) << std::endl;
    unInitMmapStreaming();
    throw std::runtime_error(_device + ": Error stopping streaming: " +
                             std::strerror(errno));
  }
}

void
Camera::unInitMmapStreaming() {
  if (_verbose) {
    std::cout << "(VERBOSE) " << _device << ": deallocating memory"
              << std::endl;
  }
  bool success = true;
  for (auto& i : _v4l2Buffers) {
    if (::munmap(i.first, i.second)) {
      success = false;
    }
  }
  _v4l2Buffers.clear();
  if (!success) {
    throw std::runtime_error(_device + ": Error when unmapping RAW buffer: " +
                             strerror(errno));
  }
}

void
Camera::closeDevice() {
  if (_verbose) {
    std::cout << "(VERBOSE) " << _device << ": closing camera" << std::endl;
  }
  _fd = ::close(_fd);
  if (_fd < 0) {
    _fd = -1;
    throw std::runtime_error(_device + ": close fails: " +
                             std::strerror(errno));
  }
  _fd = -1;
}

v4l2_buffer
Camera::lockKernelBuffer() {
  v4l2_buffer buffer;
  buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buffer.memory = V4L2_MEMORY_MMAP;
  // wait unread image
  {
    // Build File descriptor list.
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(_fd, &fds);
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    int r = ::select(_fd + 1, &fds, NULL, NULL, &timeout);
    if (0 == r) {
      throw std::runtime_error(
          _device + ": Can't get new image from driver: timeout on select");
    } else if (-1 == r) {
      throw std::runtime_error(_device + ": Can't get new image from driver: " +
                               std::strerror(errno));
    }
  }

  if (-1 == ::ioctl(_fd, VIDIOC_DQBUF, &buffer)) {
    std::cerr << "(ERROR) " << _device << ": VIDIOC_DQBUF ("
              << std::to_string(_IOC_NR(VIDIOC_DQBUF))
              << "): " << std::strerror(errno) << std::endl;
    throw std::runtime_error(
        _device + ": Cannot dequeue v4l2 buffer from kernel ring buffer: " +
        strerror(errno));
  }

  if (_verbose) {
    std::cout << "(VERBOSE) " << _device << ": lock buffer,"
              << " timecode: " << std::setw(3) << int32_t(buffer.sequence)
              << "seq"
              << " timestamp: " << std::setw(4) << buffer.timestamp.tv_sec
              << "s, " << std::setw(3) << buffer.timestamp.tv_usec / 1000
              << "ms" << std::endl;
  }
  return buffer;
}

void
Camera::releaseKernelBuffer(v4l2_buffer& buffer) {
  if (_verbose) {
    std::cout << "(VERBOSE) " << _device << ": release buffer,"
              << " timecode: " << std::setw(3) << int32_t(buffer.sequence)
              << "seq" << std::endl;
  }
  if (-1 == ::ioctl(_fd, VIDIOC_QBUF, &buffer)) {
    std::cerr << "(ERROR) " << _device << ": VIDIOC_QBUF ("
              << std::to_string(_IOC_NR(VIDIOC_QBUF))
              << "): " << std::strerror(errno) << std::endl;
    throw std::runtime_error(_device + ": Cannot release v4l2 buffer: " +
                             strerror(errno));
  }
}

void
Camera::flushBuffer() {
  v4l2_buffer buffer;
  buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buffer.memory = V4L2_MEMORY_MMAP;

  int result = ::ioctl(_fd, VIDIOC_DQBUF, &buffer);
  while (0 == result) {
    if (_verbose) {
      std::cout << "(VERBOSE) " << _device << ": lock and release buffer, "
                << " timecode: " << std::setw(3) << int32_t(buffer.sequence)
                << "seq" << std::endl;
    }
    if (-1 == ::ioctl(_fd, VIDIOC_QBUF, &buffer)) {
      std::cerr << "(ERROR) " << _device << ": VIDIOC_QBUF ("
                << std::to_string(_IOC_NR(VIDIOC_QBUF))
                << "): " << std::strerror(errno) << std::endl;
      throw std::runtime_error(_device + ": Cannot release v4l2 buffer: " +
                               strerror(errno));
    }
    result = ::ioctl(_fd, VIDIOC_DQBUF, &buffer);
  }
}

std::unique_ptr<std::uint8_t>
Camera::getImage() {
  v4l2_buffer buffer = lockKernelBuffer();
  std::unique_ptr<std::uint8_t> result(new std::uint8_t[_width * _height * 2]);
  std::memcpy(result.get(), _v4l2Buffers[buffer.index].first,
              _width * _height * 2);
  releaseKernelBuffer(buffer);
  return result;
}

void
Camera::printBuffer(v4l2_buffer& buffer) const {
  if (!::isatty(::fileno(stdout))) {
    void* data = _v4l2Buffers[buffer.index].first;
    size_t size = _v4l2Buffers[buffer.index].second;
    ssize_t size_out = size;
    size_out = fwrite(data, size, 1, stdout);
    if (size_out != 1) {
      throw std::runtime_error(std::strerror(errno));
    }
  }
}

static inline std::uint8_t
clamp(int v) {
  return std::uint8_t(std::uint32_t(v) <= 255 ? v : v > 0 ? 255 : 0);
}

//! @brief convert yuv tuple to rgb.
static inline void
yuv2rgb(const int y,
        const int u,
        const int v,
        unsigned char& r,
        unsigned char& g,
        unsigned char& b) {
  int c, d, e;
  c = y - 16;
  d = u - 128;
  e = v - 128;
  r = clamp((298 * c + 409 * e + 128) >> 8);
  g = clamp((298 * c - 100 * d - 208 * e + 128) >> 8);
  b = clamp((298 * c + 516 * d + 128) >> 8);
}

void
Camera::writePPM(const std::string& fileName) {
  std::unique_ptr<std::uint8_t> buffer = getImage();
  std::uint8_t* data = buffer.get();

  if (_verbose) {
    std::cout << "(VERBOSE) " << _device << ": write image " << fileName
              << std::endl;
  }
  std::ofstream ofs;
  ofs.open(fileName, std::ofstream::out | std::ios_base::binary);
  ofs << "P6 " << _width << " " << _height << " 255 ";
  for (std::uint32_t j = 0; j < _height; ++j) {
    for (std::uint32_t i = 0; i < _width; i += 2) {
      std::uint8_t y0 = *data++;
      std::uint8_t u = *data++;
      std::uint8_t y1 = *data++;
      std::uint8_t v = *data++;

      std::uint8_t red, green, blue;
      yuv2rgb(y0, u, v, red, green, blue);
      ofs << red << green << blue;
      yuv2rgb(y1, u, v, red, green, blue);
      ofs << red << green << blue;
    }
  }
  ofs.close();
}

void
Camera::writeBuffer2PPM(const std::string& fileName,
                        std::uint32_t width,
                        std::uint32_t height,
                        std::uint8_t* data) {
  if (_verbose) {
    std::cout << "(VERBOSE) "
              << "Write image " << fileName << std::endl;
  }

  std::ofstream ofs;
  ofs.open(fileName, std::ofstream::out | std::ios_base::binary);
  ofs << "P6 " << width << " " << height << " 255 ";
  for (std::uint32_t j = 0; j < height; ++j) {
    for (std::uint32_t i = 0; i < width; i += 2) {
      std::uint8_t y0 = *data++;
      std::uint8_t u = *data++;
      std::uint8_t y1 = *data++;
      std::uint8_t v = *data++;

      std::uint8_t red, green, blue;
      yuv2rgb(y0, u, v, red, green, blue);
      ofs << red << green << blue;
      yuv2rgb(y1, u, v, red, green, blue);
      ofs << red << green << blue;
    }
  }
  ofs.close();
}

std::int32_t
CameraLIOV5640::getExtUnit(std::uint8_t reg) {
  std::uint8_t data[2];
  struct uvc_xu_control_query queryctrl;
  ::memset(&queryctrl, 0, sizeof(queryctrl));
  queryctrl.unit = 3;
  queryctrl.selector = reg;
  queryctrl.query = UVC_GET_CUR;
  queryctrl.size = 2;
  queryctrl.data = data;
  if (-1 == ::ioctl(_fd, UVCIOC_CTRL_QUERY, &queryctrl)) {
    throw std::runtime_error(_device + ": UVC_GET_CUR fails: " +
                             std::strerror(errno));
  }
  return (data[1] << 8) | data[0];
}

std::int32_t
CameraLIOV5640::setExtUnit(std::uint8_t reg, std::int32_t value) {
  std::uint8_t data[2];
  data[0] = value & 0xff;
  data[1] = (value >> 8) & 0xff;

  struct uvc_xu_control_query queryctrl;
  ::memset(&queryctrl, 0, sizeof(queryctrl));
  queryctrl.unit = 3;
  queryctrl.selector = reg;
  queryctrl.query = UVC_SET_CUR;
  queryctrl.size = 2;
  queryctrl.data = data;
  if (-1 == ::ioctl(_fd, UVCIOC_CTRL_QUERY, &queryctrl)) {
    throw std::runtime_error(_device + ": UVC_SET_CUR fails: " +
                             std::strerror(errno));
  }
}

void
CameraLIOV5640::setFlip(std::int32_t value) {
  std::int32_t hflip = getExtUnit(0x0c);
  if (_verbose) {
    std::cout << "(VERBOSE) " << _device
              << ": set horizontal flip (current: " << hflip
              << ", new:" << value << ")" << std::endl;
  }
  setExtUnit(0x0c, value);

  std::int32_t vflip = getExtUnit(0x0d);
  if (_verbose) {
    std::cout << "(VERBOSE) " << _device
              << ": set vertical flip (current: " << vflip << ", new:" << value
              << ")" << std::endl;
  }
  setExtUnit(0x0d, value);
}
