//! @file
//! Test the impact of the V4L2 ring buffer size on first images.

#include "camera.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

int
main(int argc, char* argv[]) {
	if (argc < 2) {
		getUsage(argv[0]);
		std::cout << "--dump: dump image (default off)" << std::endl;
		return 1;
	}
	bool dump = false;
	if (cmdOptionExists(argc, argv, "--dump")) dump = true;

	try {
		{ // Test One
			std::cout << "Test1: START" << std::endl;
			Camera cam(argc, argv);
			if (cmdOptionExists(argc, argv, "--info")) {
				std::cout << "(INFO) dump: " << dump << std::endl;
			}
			cam.openDevice();
			cam.setResolution();
			cam.setFrameRate();
			cam.initMmapStreaming(3);
			cam.startStreaming();

			std::this_thread::sleep_for(std::chrono::milliseconds(2000 / cam.fps()));

			std::vector<std::unique_ptr<std::uint8_t>> buffers;
			for (int i = 0; i < 8; ++i) {
				buffers.push_back(cam.getImage());
			}
			if (dump) {
				int i = 0;
				for (auto const& it : buffers) {
					std::stringstream filename;
					filename << "ring3_" << std::setw(3) << std::setfill('0') << i++ << ".ppm";
					std::cout << filename.str() << ": ";
					cam.writeBuffer2PPM(filename.str(), cam.width(), cam.height(), it.get());
				}
			}
			cam.stopStreaming();
			cam.unInitMmapStreaming();
			cam.closeDevice();
			std::cout << "Test1: DONE" << std::endl;
		}

		{ // Test Two
			std::cout << "Test2: START" << std::endl;
			Camera cam(argc, argv);
			cam.openDevice();
			cam.setResolution();
			cam.setFrameRate();
			cam.initMmapStreaming(8);
			cam.startStreaming();
			std::vector<std::unique_ptr<std::uint8_t>> buffers;
			for (int i = 0; i < 15; ++i) {
				buffers.push_back(cam.getImage());
			}
			if (dump) {
				int i = 0;
				for (auto const& it : buffers) {
					std::stringstream filename;
					filename << "ring8_" << std::setw(3) << std::setfill('0') << i++ << ".ppm";
					std::cout << filename.str() << ": ";
					cam.writeBuffer2PPM(filename.str(), cam.width(), cam.height(), it.get());
				}
			}
			cam.stopStreaming();
			cam.unInitMmapStreaming();
			cam.closeDevice();
			std::cout << "Test2: DONE" << std::endl;
		}

		// Test Three
		{
			std::cout << "Test3: START" << std::endl;
			Camera cam(argc, argv);
			cam.openDevice();
			cam.setResolution();
			cam.setFrameRate();
			cam.initMmapStreaming(8);
			cam.startStreaming();
			cam.flushBuffer();
			std::vector<std::unique_ptr<std::uint8_t>> buffers;
			for (int i = 0; i < 15; ++i) {
				buffers.push_back(cam.getImage());
			}
			if (dump) {
				int i = 0;
				for (auto const& it : buffers) {
					std::stringstream filename;
					filename << "ring8flush_" << std::setw(3) << std::setfill('0') << i++
					         << ".ppm";
					std::cout << filename.str() << ": ";
					cam.writeBuffer2PPM(filename.str(), cam.width(), cam.height(), it.get());
				}
			}
			cam.stopStreaming();
			cam.unInitMmapStreaming();
			cam.closeDevice();
			std::cout << "Test3: DONE" << std::endl;
		}

		// Test Four
		{
			std::cout << "Test4: START" << std::endl;
			Camera cam(argc, argv);
			cam.openDevice();
			cam.setResolution();
			cam.setFrameRate();
			cam.initMmapStreaming(8);
			cam.startStreaming();
			std::this_thread::sleep_for(std::chrono::milliseconds(1000 / cam.fps()));
			cam.flushBuffer();
			std::vector<std::unique_ptr<std::uint8_t>> buffers;
			for (int i = 0; i < 15; ++i) {
				buffers.push_back(cam.getImage());
			}
			if (dump) {
				int i = 0;
				for (auto const& it : buffers) {
					std::stringstream filename;
					filename << "ring8flush_" << std::setw(3) << std::setfill('0') << i++
					         << ".ppm";
					std::cout << filename.str() << ": ";
					cam.writeBuffer2PPM(filename.str(), cam.width(), cam.height(), it.get());
				}
			}
			cam.stopStreaming();
			cam.unInitMmapStreaming();
			cam.closeDevice();
			std::cout << "Test4: DONE" << std::endl;
		}
	} catch (std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
		return 2;
	}
	return 0;
}
