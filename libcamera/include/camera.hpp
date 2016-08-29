#pragma once

extern "C" {
#include <linux/videodev2.h> // v4l2_buffer
}
#include <memory>
#include <string>
#include <vector>

#include <algorithm>
std::string
getCmdOption(int argc, char* argv[], const std::string& option) {
	std::vector<std::string> args(argv, argv + argc);
	std::vector<std::string>::const_iterator itr =
	  std::find(args.begin(), args.end(), option);
	if (itr != args.end() && ++itr != args.end()) {
		return *itr;
	}
	return std::string();
}

bool
cmdOptionExists(int argc, char* argv[], const std::string& option) {
	std::vector<std::string> args(argv, argv + argc);
	return std::find(args.begin(), args.end(), option) != args.end();
}

#include <iostream>
void
getUsage(const std::string& name) {
	std::cout << "usage: " << name << " -d /dev/video0" << std::endl
	          << "-d name: device name (default /dev/video0)" << std::endl
	          << "-w value: width (default 640)" << std::endl
	          << "-h value: height (default 480)" << std::endl
	          << "-fps value: framerate (default 15)" << std::endl
	          << "-nb value: v4l2_ring_buffer_size (default 4)" << std::endl
	          << "--info: get configuration info (default off)" << std::endl
	          << "--verbose: activate verbose log (default off)" << std::endl;
}

class Camera {
	public:
	Camera(int argc, char** argv);
	virtual ~Camera();

	void getInfo() const;

	void openDevice();
	void openDevice(const std::string& deviceName);

	void setResolution();
	void setResolution(std::uint32_t w, std::uint32_t h);

	void setFrameRate();
	void setFrameRate(uint32_t frame);

	void initMmapStreaming();
	void initMmapStreaming(int bufferCount);

	void startStreaming();

	void stopStreaming();

	void unInitMmapStreaming();

	void closeDevice();

	v4l2_buffer lockKernelBuffer();

	void releaseKernelBuffer(v4l2_buffer& buffer);

	void flushBuffer();

	std::unique_ptr<uint8_t> getImage();

	void writePPM(const std::string& fileName);

	void writeBuffer2PPM(const std::string& fileName,
	                     std::uint32_t w,
	                     std::uint32_t h,
	                     std::uint8_t* buffer);

	void printBuffer(v4l2_buffer& buffer) const;

	inline std::uint32_t width() const noexcept { return _width; }
	inline std::uint32_t height() const noexcept { return _height; }
	inline std::uint32_t fps() const noexcept { return _fps; }
	inline std::uint32_t bufferNb() const noexcept { return _bufferNb; }

	protected:
	std::string _device;
	std::uint32_t _width;
	std::uint32_t _height;
	std::uint32_t _fps;
	std::uint32_t _bufferNb;
	bool _verbose;

	std::int32_t _fd;
	std::vector<std::pair<void*, std::uint32_t>> _v4l2Buffers;
};

class CameraLIOV5640 : public Camera {
	public:
	CameraLIOV5640(int argc, char** argv)
	  : Camera(argc, argv) {}
	virtual ~CameraLIOV5640() = default;

	void setFlip(std::int32_t value);
	void setAutoExposure(bool enable);
	void setExposure(std::int32_t value);
	void setGain(std::int32_t value);

	protected:
	std::int32_t getExtUnit(std::uint8_t reg);
	std::int32_t setExtUnit(std::uint8_t reg, std::int32_t value);
};
