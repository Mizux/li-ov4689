//! @file
//! Do N times (default 100):
//!  - Open device
//!  - Init...
//!  - Grabe M images (default 15)
//!  - Close device.

#include "camera.hpp"

#include <thread>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdlib>

int
main(int argc, char* argv[]) {
  if (argc < 2) {
    getUsage(argv[0]);
    std::cout << "--dump: dump image (default off)" << std::endl;
    std::cout << "-loop value: number of test run (default 100)" << std::endl;
    std::cout << "-frame value: number of getImage by run (default 15)"
              << std::endl;
    return 1;
  }
  bool dump = false;
  if (cmdOptionExists(argc, argv, "--dump"))
    dump = true;

  int loop = 100;
  if (cmdOptionExists(argc, argv, "-loop")) {
    loop = std::atoi(getCmdOption(argc, argv, "-loop").c_str());
  }

  int frame = 15;
  if (cmdOptionExists(argc, argv, "-frame")) {
    frame = std::atoi(getCmdOption(argc, argv, "-frame").c_str());
  }

  Camera cam(argc, argv);
  if (cmdOptionExists(argc, argv, "--info")) {
    std::cout << "(INFO) dump: " << dump << std::endl;
    std::cout << "(INFO) loop number: " << loop << std::endl;
    std::cout << "(INFO) frame number: " << frame << std::endl;
  }
  int failCount = 0;
  for (int i = 0; i < loop; ++i) {
    try {
      cam.openDevice();
      cam.setResolution();
      cam.setFrameRate();
      cam.initMmapStreaming();
      cam.startStreaming();

      if (dump) {
        // get all images (reduce latency between call
        std::vector<std::unique_ptr<std::uint8_t>> buffers;
        for (int i = 0; i < frame; ++i) {
          buffers.push_back(cam.getImage());
        }
        // then dump them
        int i = 0;
        for (auto const& it : buffers) {
          std::stringstream filename;
          filename << "full_" << std::setw(3) << std::setfill('0') << i++
                   << ".ppm";
          cam.writeBuffer2PPM(filename.str(), cam.width(), cam.height(),
                              it.get());
        }
      } else {
        for (int i = 0; i < frame; ++i) {
          cam.getImage();
        }
      }
      cam.stopStreaming();
      cam.unInitMmapStreaming();
      cam.closeDevice();
    } catch (std::exception& e) {
      failCount++;
      std::cerr << "(ERROR) Fails in loop " << i << ": " << e.what()
                << std::endl;
      system("echo -n \"(ERROR): \";date --rfc-3339=ns");
      cam.stopStreaming();
      cam.unInitMmapStreaming();
      cam.closeDevice();
    }
  }
  if (failCount == 0) {
    std::cout << "(INFO) TEST PASS (0 fails on " << loop << " runs)"
              << std::endl;
    system("echo -n \"(INFO): \";date --rfc-3339=ns");
  } else {
    std::cout << "(INFO) TEST FAIL (" << failCount << " fails on " << loop
              << " runs)" << std::endl;
    system("echo -n \"(INFO): \";date --rfc-3339=ns");
  }
  return 0;
}
