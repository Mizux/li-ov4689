//! @file
//! Longterm test, with dump of images every 13500 frames...
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
		std::cout << "--dump: dump image every 13500 acquisition (default off)"
		          << std::endl;
		std::cout << "-loop value: number of getImage (default 13500 ~= 15min at 15fps)"
		          << std::endl;
		return 1;
	}
	bool dump = false;
	if (cmdOptionExists(argc, argv, "--dump")) dump = true;

	int loop = 13500;
	if (cmdOptionExists(argc, argv, "-loop")) {
		loop = std::atoi(getCmdOption(argc, argv, "-loop").c_str());
	}

	try {
		Camera cam(argc, argv);
		if (cmdOptionExists(argc, argv, "--info")) {
			std::cout << "(INFO) dump: " << dump << std::endl;
			std::cout << "(INFO) loop number: " << loop << std::endl;
		}
		cam.openDevice();
		cam.setResolution();
		cam.setFrameRate();
		cam.initMmapStreaming();
		cam.startStreaming();

		std::uint64_t failCount = 0;
		for (int i = 0; i < loop; ++i) {
			try {
				if (dump && ((i % 13500) < 5)) {
					// dump 5 frames every 15min (15fps * 60s_min * 15min)
					std::string filename = "image_" + std::to_string(i) + ".ppm";
					cam.writePPM(filename);
				} else {
					cam.getImage();
				}
			} catch (std::exception& e) {
				failCount++;
				std::cerr << "(ERROR) Fails in loop " << i << ": " << e.what() << std::endl;
			}
		}
		cam.stopStreaming();
		cam.unInitMmapStreaming();
		cam.closeDevice();
		if (failCount == 0)
			std::cout << "(INFO) TEST PASS (0 fails on " << loop << " runs)" << std::endl;
		else
			std::cout << "(INFO) TEST FAIL (" << failCount << " fails on " << loop << " runs)"
			          << std::endl;
	} catch (std::exception& e) {
		std::cerr << "(ERROR): Can't start streaming: " << e.what() << std::endl;
		return 2;
	}
	return 0;
}
