//! @file
//! Grabbe N images (default 200) then write them to disk.

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
		std::cout << "-loop value: number of test run (default 200)" << std::endl;
		return 1;
	}
	bool dump = false;
	if (cmdOptionExists(argc, argv, "--dump")) dump = true;

	int loop = 200;
	if (cmdOptionExists(argc, argv, "-loop")) {
		loop = std::atoi(getCmdOption(argc, argv, "-loop").c_str());
	}

	std::cout << "(INFO) Test...START" << std::endl;
	std::vector<std::unique_ptr<std::uint8_t>> buffers;

	Camera cam(argc, argv);
	if (cmdOptionExists(argc, argv, "--info")) {
		std::cout << "(INFO) dump: " << dump << std::endl;
		std::cout << "(INFO) loop number: " << loop << std::endl;
	}
	try {
		cam.openDevice();
		cam.setResolution();
		cam.setFrameRate();

		cam.initMmapStreaming();
		cam.startStreaming();
		// Acquisition
		for (int i = 0; i < loop; ++i) {
			buffers.push_back(cam.getImage());
		}
		cam.stopStreaming();
		cam.unInitMmapStreaming();
		cam.closeDevice();
	} catch (std::exception& e) {
		std::cout << "(ERROR) " << e.what() << std::endl;
		return 2;
	}
	std::cout << "(INFO) Test...DONE" << std::endl;

	if (dump) {
		std::cout << "(INFO) Dump result...START" << std::endl;
		// Dump to disk
		int i = 0;
		for (auto const& it : buffers) {
			try {
				std::stringstream filename;
				filename << "image_" << std::setw(3) << std::setfill('0') << i++ << ".ppm";
				cam.writeBuffer2PPM(filename.str(), cam.width(), cam.height(), it.get());
			} catch (const std::exception& e) {
				std::cout << "(ERROR) Error in loop " << i - 1 << ": " << e.what() << std::endl;
			}
		}
		std::cout << "(INFO) Dump result...DONE" << std::endl;
	}
	buffers.clear();

	return 0;
}
