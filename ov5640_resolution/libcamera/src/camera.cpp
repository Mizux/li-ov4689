
#include "camera.hpp"

extern "C" {
#include <fcntl.h>            // open
#include <unistd.h>           // close
#include <sys/mman.h>         // mmap
#include <sys/ioctl.h>        // ioctl
#include <linux/uvcvideo.h>   // UVC ExtUnit control
#include <linux/usb/video.h>  // define UVC_*
}
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <cstring>  // strerror
#include <fstream>  // std::ofstream

Camera::Camera()
    : device("/dev/video0"), width(0), height(0), fps(1), bufferNb(0), fd(-1) {}

Camera::~Camera() {
  if (fd != -1) {
    stopStreaming(true);
    unInitMmapStreaming(true);
    closeDevice(true);
  }
}

extern "C" {
//! @brief Small helper to remove warning (old style cast)
static inline std::uint32_t getYUYVformat() {
  return V4L2_PIX_FMT_YUYV;
}
//! @brief small helper to remove warning (old style cast)
static const void* mapFailed = MAP_FAILED;
}

void Camera::openDevice(const std::string& deviceName, bool enableTrace) {
  if (enableTrace) {
    std::cout << deviceName << ": open device" << std::endl;
  }
  // fd = ::open(deviceName.c_str(), O_RDWR | O_CLOEXEC);
  fd = ::open(deviceName.c_str(), O_RDWR | O_NONBLOCK | O_CLOEXEC);
  if (fd < 0) {
    fd = -1;
    throw std::runtime_error(deviceName + ": open fails: " +
                             std::strerror(errno));
  }
  device = deviceName;
}

void Camera::setResolution(uint32_t w, uint32_t h, bool enableTrace) {
  if (enableTrace) {
    std::cout << device << ": set resolution to " << w << "x" << h << std::endl;
  }
  struct v4l2_format fmt;
  ::memset(&fmt, 0, sizeof(fmt));
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  // for a video device
  fmt.fmt.pix.field = V4L2_FIELD_NONE;     // not interlaced camera
  fmt.fmt.pix.width = w;
  fmt.fmt.pix.height = h;
  fmt.fmt.pix.pixelformat = getYUYVformat();
  fmt.fmt.pix.colorspace = V4L2_COLORSPACE_JPEG;
  if (-1 == ::ioctl(fd, VIDIOC_S_FMT, &fmt)) {
    throw std::runtime_error(device + ": VIDIOC_S_FMT fails: " +
                             std::strerror(errno));
  }
  width = w;
  height = h;
}

void Camera::setFrameRate(std::uint32_t frame, bool enableTrace) {
  if (enableTrace) {
    std::cout << device << ": set framerate to " << frame << std::endl;
  }
  struct v4l2_streamparm parm;
  ::memset(&parm, 0, sizeof(parm));
  parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  parm.parm.capture.timeperframe.numerator = 1;
  parm.parm.capture.timeperframe.denominator = frame;
  // indicates that the device can vary its frame rate
  parm.parm.capture.capability = V4L2_CAP_TIMEPERFRAME;
  parm.parm.capture.extendedmode = 0;
  if (-1 == ::ioctl(fd, VIDIOC_S_PARM, &parm)) {
    throw std::runtime_error(device + ": VIDIOC_S_PARM fails: " +
                             std::strerror(errno));
  }
  fps = frame;
}

void Camera::initMmapStreaming(int bufferCount, bool enableTrace) {
  bufferNb = bufferCount;
  if (enableTrace) {
    std::cout << device << ": allocating memory (" << bufferNb << " buffers)"
              << std::endl;
  }
  // First request a number of buffer.
  struct v4l2_requestbuffers req;
  ::memset(&req, 0, sizeof(req));
  req.count = bufferNb;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;
  if (-1 == ::ioctl(fd, VIDIOC_REQBUFS, &req)) {
    throw std::runtime_error(device + ": VIDIOC_REQBUFS fails: " +
                             std::strerror(errno));
  }
  if (req.count != bufferNb) {
    throw std::runtime_error(device +
                             ": Insufficient memory to create "
                             "enough RAW buffer in the kernel "
                             "ring buffer.");
  }

  // Get pointer and size of each one
  for (auto i = 0; i < bufferNb; ++i) {
    struct v4l2_buffer buf;
    ::memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;
    if (-1 == ::ioctl(fd, VIDIOC_QUERYBUF, &buf)) {
      throw std::runtime_error(device + ": VIDIOC_QUERYBUF fails: " +
                               std::to_string(i) + ": " + std::strerror(errno));
    }

    // MMAP them
    void* startAddr = ::mmap(NULL, buf.length, PROT_READ | PROT_WRITE,
                             MAP_SHARED, fd, buf.m.offset);
    if (mapFailed == startAddr) {
      std::cout << device << ": Can't init mmap: "
                << "mmap error: "
                << " fd: " << fd << " buffer offset: " << std::hex
                << buf.m.offset << std::dec << " length: " << buf.length
                << std::endl;
      throw std::runtime_error(device + ": Cannot mmap kernel buffer " +
                               std::to_string(i) + ": " + strerror(errno));
    }
    v4l2Buffers.push_back(std::make_pair(startAddr, buf.length));

    // Enqueue them
    if (-1 == ::ioctl(fd, VIDIOC_QBUF, &buf)) {
      throw std::runtime_error(device + ": VIDIOC_QBUF fails: " +
                               std::to_string(i) + ": " + strerror(errno));
    }
  }  // end of For
}

void Camera::startStreaming(bool enableTrace) {
  if (enableTrace) {
    std::cout << device << ": start streaming" << std::endl;
  }
  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (-1 == ::ioctl(fd, VIDIOC_STREAMON, &type)) {
    throw std::runtime_error(device + ": Cannot start streaming: " +
                             std::strerror(errno));
  }
}

void Camera::stopStreaming(bool enableTrace) {
  if (enableTrace) {
    std::cout << device << ": stop streaming" << std::endl;
  }
  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (-1 == ::ioctl(fd, VIDIOC_STREAMOFF, &type)) {
    unInitMmapStreaming();
    throw std::runtime_error(device + ": Error stopping streaming: " +
                             std::strerror(errno));
  }
}

void Camera::unInitMmapStreaming(bool enableTrace) {
  if (enableTrace) {
    std::cout << device << ": deallocating memory" << std::endl;
  }
  bool success = true;
  for (auto& i : v4l2Buffers) {
    if (::munmap(i.first, i.second)) {
      success = false;
    }
  }
  v4l2Buffers.clear();
  if (!success) {
    throw std::runtime_error(device + ": Error when unmapping RAW buffer: " +
                             strerror(errno));
  }
}

void Camera::closeDevice(bool enableTrace) {
  if (enableTrace) {
    std::cout << device << ": closing camera" << std::endl;
  }
  fd = ::close(fd);
  if (fd < 0) {
    fd = -1;
    throw std::runtime_error(device + ": close fails: " + std::strerror(errno));
  }
  fd = -1;
}

v4l2_buffer Camera::lockKernelBuffer(bool enableTrace) {
  v4l2_buffer buffer;
  buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buffer.memory = V4L2_MEMORY_MMAP;
  // wait unread image
  {
    // Build File descriptor list.
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
    int r = ::select(fd + 1, &fds, NULL, NULL, &timeout);
    if (0 == r) {
      throw std::runtime_error(device +
                               ": Can't get new image from driver: timeout");
    } else if (-1 == r) {
      throw std::runtime_error(device + ": Can't get new image from driver: " +
                               std::strerror(errno));
    }
  }

  if (-1 == ::ioctl(fd, VIDIOC_DQBUF, &buffer)) {
    throw std::runtime_error(
        device + ": Cannot dequeue v4l2 buffer from kernel ring buffer: " +
        strerror(errno));
  }

  if (enableTrace) {
    std::cout << device << ": lock buffer,"
              << " timecode: " << std::setw(3) << int32_t(buffer.sequence)
              << "seq"
              << " timestamp: " << std::setw(4) << buffer.timestamp.tv_sec
              << "s, " << std::setw(3) << buffer.timestamp.tv_usec / 1000
              << "ms" << std::endl;
  }
  return buffer;
}

void Camera::releaseKernelBuffer(v4l2_buffer& buffer, bool enableTrace) {
  if (enableTrace) {
    std::cout << device << ": release buffer,"
              << " timecode: " << std::setw(3) << int32_t(buffer.sequence)
              << "seq" << std::endl;
  }
  if (-1 == ::ioctl(fd, VIDIOC_QBUF, &buffer)) {
    throw std::runtime_error(device + ": Cannot release v4l2 buffer: " +
                             strerror(errno));
  }
}

void Camera::flushBuffer(bool enableTrace) {
  v4l2_buffer buffer;
  buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buffer.memory = V4L2_MEMORY_MMAP;

  int result = ::ioctl(fd, VIDIOC_DQBUF, &buffer);
  while (0 == result) {
    if (enableTrace) {
      std::cout << device << ": lock and release buffer, "
                << " timecode: " << std::setw(3) << int32_t(buffer.sequence)
                << "seq" << std::endl;
    }
    if (-1 == ::ioctl(fd, VIDIOC_QBUF, &buffer)) {
      throw std::runtime_error(device + ": Cannot release v4l2 buffer: " +
                               strerror(errno));
    }
    result = ::ioctl(fd, VIDIOC_DQBUF, &buffer);
  }
}

std::unique_ptr<std::uint8_t> Camera::getImage(bool enableTrace) {
  v4l2_buffer buffer = lockKernelBuffer(enableTrace);
  std::unique_ptr<std::uint8_t> result(new std::uint8_t[width * height * 2]);
  std::memcpy(result.get(), v4l2Buffers[buffer.index].first,
              width * height * 2);
  releaseKernelBuffer(buffer, enableTrace);
  return result;
}

void Camera::printBuffer(v4l2_buffer& buffer) const {
  if (!::isatty(::fileno(stdout))) {
    void* data = v4l2Buffers[buffer.index].first;
    size_t size = v4l2Buffers[buffer.index].second;
    ssize_t size_out = size;
    size_out = fwrite(data, size, 1, stdout);
    if (size_out != 1) {
      throw std::runtime_error(std::strerror(errno));
    }
  }
}

static inline std::uint8_t clamp(int v) {
  return std::uint8_t(std::uint32_t(v) <= 255 ? v : v > 0 ? 255 : 0);
}

//! @brief convert yuv tuple to rgb.
static inline void yuv2rgb(const int y,
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

void Camera::writePPM(const std::string& fileName, bool enableTrace) {
  std::unique_ptr<std::uint8_t> buffer = getImage(enableTrace);
  std::uint8_t* data = buffer.get();

  if (enableTrace) {
    std::cout << device << ": write image " << fileName << std::endl;
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

void Camera::writeBuffer2PPM(const std::string& fileName,
                             std::uint32_t width,
                             std::uint32_t height,
                             std::uint8_t* data,
                             bool enableTrace) {
  if (enableTrace) {
    std::cout << "Write image " << fileName << std::endl;
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

std::int32_t CameraLIOV5640::getExtUnit(std::uint8_t reg) {
  std::uint8_t data[2];
  struct uvc_xu_control_query queryctrl;
  ::memset(&queryctrl, 0, sizeof(queryctrl));
  queryctrl.unit = 3;
  queryctrl.selector = reg;
  queryctrl.query = UVC_GET_CUR;
  queryctrl.size = 2;
  queryctrl.data = data;
  if (-1 == ::ioctl(fd, UVCIOC_CTRL_QUERY, &queryctrl)) {
    throw std::runtime_error(device + ": UVC_GET_CUR fails: " +
                             std::strerror(errno));
  }
  return (data[1] << 8) | data[0];
}

std::int32_t CameraLIOV5640::setExtUnit(std::uint8_t reg, std::int32_t value) {
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
  if (-1 == ::ioctl(fd, UVCIOC_CTRL_QUERY, &queryctrl)) {
    throw std::runtime_error(device + ": UVC_SET_CUR fails: " +
                             std::strerror(errno));
  }
}

void CameraLIOV5640::setFlip(std::int32_t value, bool enableTrace) {
  std::int32_t hflip = getExtUnit(0x0c);
  if (enableTrace) {
    std::cout << device << ": set horizontal flip (current: " << hflip
              << ", new:" << value << ")" << std::endl;
  }
  setExtUnit(0x0c, value);

  std::int32_t vflip = getExtUnit(0x0d);
  if (enableTrace) {
    std::cout << device << ": set vertical flip (current: " << vflip
              << ", new:" << value << ")" << std::endl;
  }
  setExtUnit(0x0d, value);
}
