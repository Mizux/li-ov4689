#include "camera.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

int
main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cout << "usage: " << argv[0] << " /dev/video0 [width height [fps]]"
		          << std::endl;
		return 1;
	}
	int width  = argc >= 4 ? std::atoi(argv[2]) : 2560;
	int height = argc >= 4 ? std::atoi(argv[3]) : 720;
	int fps    = argc >= 5 ? std::stoi(argv[4]) : 15;

	try {
		// Test One
		{
			std::cout << "Test1: START" << std::endl;
			Camera cam;
			cam.openDevice(argv[1], true);
			cam.setResolution(width, height, true);
			cam.setFrameRate(fps, true);
			cam.initMmapStreaming(3, true);
			cam.startStreaming(true);
			std::vector<std::unique_ptr<std::uint8_t>> buffers;
			for (int i = 0; i < 5; ++i) {
				buffers.push_back(cam.getImage(true));
			}
			int i = 0;
			for (auto const& it : buffers) {
				std::stringstream filename;
				filename << "ring3_" << std::setw(3) << std::setfill('0') << i++ << ".ppm";
				std::cout << filename.str() << ": ";
				Camera::writeBuffer2PPM(filename.str(), width, height, it.get(), true);
			}
			cam.stopStreaming(true);
			cam.unInitMmapStreaming(true);
			cam.closeDevice(true);
			std::cout << "Test1: DONE" << std::endl;
		}

		// Test Two
		{
			std::cout << "Test2: START" << std::endl;
			Camera cam;
			cam.openDevice(argv[1], true);
			cam.setResolution(width, height, true);
			cam.setFrameRate(fps, true);
			cam.initMmapStreaming(8, true);
			cam.startStreaming(true);
			std::vector<std::unique_ptr<std::uint8_t>> buffers;
			for (int i = 0; i < 15; ++i) {
				buffers.push_back(cam.getImage(true));
			}
			int i = 0;
			for (auto const& it : buffers) {
				std::stringstream filename;
				filename << "ring8_" << std::setw(3) << std::setfill('0') << i++ << ".ppm";
				std::cout << filename.str() << ": ";
				Camera::writeBuffer2PPM(filename.str(), width, height, it.get(), true);
			}
			cam.stopStreaming(true);
			cam.unInitMmapStreaming(true);
			cam.closeDevice(true);
			std::cout << "Test2: DONE" << std::endl;
		}

		// Test Three
		{
			std::cout << "Test3: START" << std::endl;
			Camera cam;
			cam.openDevice(argv[1], true);
			cam.setResolution(width, height, true);
			cam.setFrameRate(fps, true);
			cam.initMmapStreaming(8, true);
			cam.startStreaming(true);
			std::this_thread::sleep_for(std::chrono::milliseconds(1000 / cam.fps));
			cam.flushBuffer(true);
			std::vector<std::unique_ptr<std::uint8_t>> buffers;
			for (int i = 0; i < 15; ++i) {
				buffers.push_back(cam.getImage(true));
			}
			int i = 0;
			for (auto const& it : buffers) {
				std::stringstream filename;
				filename << "ring8flush_" << std::setw(3) << std::setfill('0') << i++ << ".ppm";
				std::cout << filename.str() << ": ";
				Camera::writeBuffer2PPM(filename.str(), width, height, it.get(), true);
			}
			cam.stopStreaming(true);
			cam.unInitMmapStreaming(true);
			cam.closeDevice(true);
			std::cout << "Test3: DONE" << std::endl;
		}
	} catch (std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
		return 2;
	}
	return 0;
}
