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

	std::cerr << "Test...START" << std::endl;
	std::vector<std::unique_ptr<std::uint8_t>> buffers;
	try {
		Camera cam;
		cam.openDevice(argv[1], true);
		cam.setResolution(width, height, true);
		cam.setFrameRate(fps, true);

		cam.initMmapStreaming(3, true);
		cam.startStreaming(true);
		// Acquisition
		for (int i = 0; i < 200; ++i) {
			buffers.push_back(cam.getImage(true));
		}
		cam.stopStreaming(true);
		cam.unInitMmapStreaming(true);
		cam.closeDevice(true);
	} catch (std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
		return 2;
	}
	std::cerr << "Test...DONE" << std::endl;

	std::cerr << "Dump result...START" << std::endl;
	// Dump to disk
	int i = 0;
	for (auto const& it : buffers) {
		try {
			std::stringstream filename;
			filename << "image_" << std::setw(3) << std::setfill('0') << i++ << ".ppm";
			Camera::writeBuffer2PPM(filename.str(), width, height, it.get());
		} catch (const std::exception& e) {
			std::cout << "Error in loop " << i - 1 << ": " << e.what() << std::endl;
		}
	}
	buffers.clear();
	std::cerr << "Dump result...DONE" << std::endl;

	return 0;
}
