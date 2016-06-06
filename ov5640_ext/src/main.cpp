#include "camera.hpp"

#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

enum {
  // control extension units
  HORIZONTAL_FLIP = 0x0c,
  VERTICAL_FLIP = 0x0d,
  TEST_PATTERN = 0x0a,
  AVERAGE_LUMINANCE = 0x08
};

int
main(int argc, char* argv[]) {
  if (argc < 2) {
    getUsage(argv[0]);
    std::cout << "-loop value: number of test run (default 100)" << std::endl;
    return 1;
  }

  int loop = 100;
  if (cmdOptionExists(argc, argv, "-loop")) {
    loop = std::atoi(getCmdOption(argc, argv, "-loop").c_str());
  }

  CameraLIOV5640 cam(argc, argv);
  for (int i = 0; i < loop; ++i) {
    try {
      std::int32_t value;
      cam.openDevice();

      const int numRegs(3);
      int regs[numRegs] = {HORIZONTAL_FLIP, VERTICAL_FLIP, TEST_PATTERN};
      for (int i = 0; i < numRegs; ++i) {
        value = cam.getExtUnit(regs[i]);
        cam.setExtUnit(regs[i], value);
      }

      value = cam.getExtUnit(AVERAGE_LUMINANCE); // read only
    } catch (std::exception& e) {
      std::cerr << "(ERROR) Fails in loop " << i << ": " << e.what()
                << std::endl;
    }
    cam.closeDevice();
  }
  return 0;
}
