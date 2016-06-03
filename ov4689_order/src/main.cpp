//! @file
//! Test what happen when no setFrameRate (i.e. VIDIOC_S_PARM) is call after a setResolution (i.e. VIDIOC_S_FMT)
#include "camera.hpp"

#include <thread>
#include <chrono>
#include <iostream>

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
    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000 * (cam.bufferNb()+2) / cam.fps())); // Fill all buffer
    cam.flushBuffer();
    cam.writePPM("FpsAfterRes.ppm");

    cam.stopStreaming();
    cam.setResolution(); // no setFps after setResolution
    cam.startStreaming();

    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000 * (cam.bufferNb()+2) / cam.fps())); // Fill all buffer
    cam.writePPM("FpsAfterRes.ppm");

    cam.stopStreaming();
    cam.unInitMmapStreaming();
    cam.closeDevice();
  } catch (std::exception& e) {
    std::cerr << "(ERROR) " << e.what() << std::endl;
    return 2;
  }
  return 0;
}
