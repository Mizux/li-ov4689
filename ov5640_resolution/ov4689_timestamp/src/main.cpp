#include "camera.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

int
main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cout << "usage: " << argv[0] << " /dev/video0" << std::endl;
		return 1;
	}
	int width  = argc >= 4 ? std::atoi(argv[2]) : 2560;
	int height = argc >= 4 ? std::atoi(argv[3]) : 720;
	int fps    = argc >= 5 ? std::stoi(argv[4]) : 15;

	try {
		Camera cam;
		cam.openDevice(argv[1], true);
		cam.setResolution(width, height, true);
		cam.setFrameRate(fps, true);

		cam.initMmapStreaming(4, true);
		cam.startStreaming(true);

		int prevSecond = 0;
		int prevMs     = 0;
		for (int i = 0; i < 500; ++i) {
			try {
				v4l2_buffer v4l2Buffer = cam.lockKernelBuffer(true);

				// Compute delta timestamp
				int delta = static_cast<int>(v4l2Buffer.timestamp.tv_sec * 1000 +
				                             v4l2Buffer.timestamp.tv_usec / 1000) -
				            static_cast<int>(prevSecond * 1000 + prevMs);
				std::cout << "loop: " << std::setw(3) << std::setfill('0') << i
				          << " delta: " << std::setw(3) << delta << "ms"
				          << " (buffer timestamp: " << std::setw(4)
				          << v4l2Buffer.timestamp.tv_sec << "s, " << std::setw(3)
				          << v4l2Buffer.timestamp.tv_usec / 1000 << "ms"
				          << " timecode: " << std::setw(3) << int32_t(v4l2Buffer.sequence)
				          << " seq)" << std::endl;
				prevSecond = v4l2Buffer.timestamp.tv_sec;
				prevMs     = v4l2Buffer.timestamp.tv_usec / 1000;

				cam.releaseKernelBuffer(v4l2Buffer, true);
			} catch (const std::exception& e) {
				std::cout << "Error in loop " << std::setw(3) << std::setfill('0') << i << ": "
				          << e.what();
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1000 / cam.fps + 66));
		}
		cam.stopStreaming(true);
		cam.unInitMmapStreaming(true);

		cam.closeDevice(true);
	} catch (std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
		return 2;
	}
	return 0;
}
