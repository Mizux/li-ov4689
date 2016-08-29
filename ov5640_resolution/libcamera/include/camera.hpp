#pragma once

extern "C" {
#include <linux/videodev2.h> // v4l2_buffer
}
#include <memory>
#include <string>
#include <vector>

class Camera {
	public:
	Camera();
	virtual ~Camera();

	void openDevice(const std::string& deviceName, bool enableTrace = false);
	void setResolution(std::uint32_t w, std::uint32_t h, bool enableTrace = false);
	void setFrameRate(uint32_t frame, bool enableTrace = false);
	void initMmapStreaming(int bufferCount, bool enableTrace = false);
	void startStreaming(bool enableTrace = false);
	void stopStreaming(bool enableTrace = false);
	void unInitMmapStreaming(bool enableTrace = false);
	void closeDevice(bool enableTrace = false);

	v4l2_buffer lockKernelBuffer(bool enableTrace = false);
	void releaseKernelBuffer(v4l2_buffer& buffer, bool enableTrace = false);
	void flushBuffer(bool enableTrace = false);

	std::unique_ptr<uint8_t> getImage(bool enableTrace = false);
	void writePPM(const std::string& fileName, bool enableTrace = false);
	static void writeBuffer2PPM(const std::string& fileName,
	                            std::uint32_t w,
	                            std::uint32_t h,
	                            std::uint8_t* buffer,
	                            bool enableTrace = false);

	void printBuffer(v4l2_buffer& buffer) const;

	std::string device;
	std::uint32_t width;
	std::uint32_t height;
	std::uint32_t fps;
	std::uint32_t bufferNb;

	protected:
	std::int32_t fd;
	std::vector<std::pair<void*, std::uint32_t>> v4l2Buffers;
};

class CameraLIOV5640 : public Camera {
	public:
	CameraLIOV5640()          = default;
	virtual ~CameraLIOV5640() = default;

	void setFlip(int32_t value, bool enableTrace = false);

	protected:
	std::int32_t getExtUnit(std::uint8_t reg);
	std::int32_t setExtUnit(std::uint8_t reg, std::int32_t value);
};
