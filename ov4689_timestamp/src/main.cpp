//! @file
//! Test timestamp integrity between acquisitions...
#include "camera.hpp"

#include <thread>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <sstream>

int main(int argc, char* argv[]) {
  if (argc < 2) {
    getUsage(argv[0]);
    return 1;
  }

  try {
    Camera cam(argc, argv);
    cam.openDevice();
    cam.setResolution();
    cam.setFrameRate();
    cam.initMmapStreaming();
    cam.startStreaming();

    int prevSecond = 0;
    int prevMs = 0;
    for (int i=0; i<500; ++i) {
      try {
      v4l2_buffer v4l2Buffer = cam.lockKernelBuffer();

      // Compute delta timestamp
      int delta = static_cast<int>(v4l2Buffer.timestamp.tv_sec * 1000 +
                                   v4l2Buffer.timestamp.tv_usec / 1000) -
                  static_cast<int>(prevSecond * 1000 + prevMs);
      std::cout << "(INFO)"
          << " loop: " << std::setw(3) << std::setfill('0') << i
          << " delta: " << std::setw(3) << delta << "ms"
          << " (buffer timestamp: " << std::setw(4) << v4l2Buffer.timestamp.tv_sec
          << "s, " << std::setw(3) << v4l2Buffer.timestamp.tv_usec / 1000 << "ms"
          << " timecode: " << std::setw(3) << int32_t(v4l2Buffer.sequence) << " seq)"
          << std::endl;
      prevSecond = v4l2Buffer.timestamp.tv_sec;
      prevMs = v4l2Buffer.timestamp.tv_usec / 1000;

      cam.releaseKernelBuffer(v4l2Buffer);
      } catch(const std::exception& e) {
        std::cerr << "(ERROR) loop " << std::setw(3) << std::setfill('0') << i
                  << ": " << e.what();
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1000 / cam.fps() + 66));
    }
    cam.stopStreaming();
    cam.unInitMmapStreaming();
    cam.closeDevice();
  } catch (std::exception& e) {
    std::cerr << "(ERROR) " << e.what() << std::endl;
    return 2;
  }
  return 0;
}
