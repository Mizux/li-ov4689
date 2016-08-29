//! @file
//! Test what happen when it take more time (+66ms) to retrieve images than
//! camera fps.

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
		std::cout << "-loop value: number of get images (default 100)" << std::endl;
		std::cout << "-delay value: delay between acquisition in addition to "
		             "framerate (default 66ms)"
		          << std::endl;
		return 1;
	}
	int loop = 100;
	if (cmdOptionExists(argc, argv, "-loop")) {
		loop = std::atoi(getCmdOption(argc, argv, "-loop").c_str());
	}
	int delay = 66;
	if (cmdOptionExists(argc, argv, "-delay")) {
		delay = std::atoi(getCmdOption(argc, argv, "-delay").c_str());
	}
	try {
		Camera cam(argc, argv);
		if (cmdOptionExists(argc, argv, "--info")) {
			std::cout << "(INFO) loop number: " << loop << std::endl;
			std::cout << "(INFO) delay: " << delay << std::endl;
		}
		std::cout << "(INFO) Test...START" << std::endl;
		cam.openDevice();
		cam.setResolution();
		cam.setFrameRate();
		cam.initMmapStreaming();
		cam.startStreaming();
		// Acquisition
		std::vector<std::unique_ptr<std::uint8_t>> buffers;
		for (int i = 0; i < loop; ++i) {
			buffers.push_back(cam.getImage());
			std::this_thread::sleep_for(std::chrono::milliseconds(1000 / cam.fps() + delay));
		}
		cam.stopStreaming();
		cam.unInitMmapStreaming();
		cam.closeDevice();
		std::cout << "(INFO) Test...DONE" << std::endl;

		std::cout << "(INFO) Dump result...START" << std::endl;
		// Dump to disk
		int i = 0;
		for (auto const& it : buffers) {
			try {
				std::stringstream filename;
				filename << "image_" << std::setw(3) << std::setfill('0') << i++ << ".ppm";
				cam.writeBuffer2PPM(filename.str(), cam.width(), cam.height(), it.get());
			} catch (const std::exception& e) {
				std::cerr << "(ERROR) loop " << i - 1 << ": " << e.what() << std::endl;
			}
		}
		buffers.clear();
		std::cout << "(INFO) Dump result...DONE" << std::endl;
	} catch (std::exception& e) {
		std::cerr << "(ERROR) " << e.what() << std::endl;
		return 2;
	}
	return 0;
}
