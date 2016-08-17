#include "camera.hpp"

#include <thread>
#include <chrono>
#include <iostream>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "usage: " << argv[0] << " /dev/video0 [width height [fps]]" << std::endl;
    return 1;
  }

  try {
    Camera cam;
    cam.openDevice(argv[1], true);
    if (argc >= 4)
      cam.setResolution(std::stoi(argv[2]), std::stoi(argv[3]), true);
    else
      cam.setResolution(2560, 720, true);
    if (argc >= 5)
      cam.setFrameRate(std::stoi(argv[4]), true);
    else
      cam.setFrameRate(15, true);

    cam.initMmapStreaming(3, true);
    cam.startStreaming(true);
    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000 * (cam.bufferNb+2) / cam.fps)); // Fill all buffer
    cam.flushBuffer(true);

    {
      cam.writePPM("FpsAfterRes.ppm", true);
    }

    cam.stopStreaming(true);
    cam.setResolution(2560, 720, true); // no setFps after setResolution
    cam.startStreaming(true);

    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000 * (cam.bufferNb+2) / cam.fps)); // Fill all buffer

    {
      cam.writePPM("FpsAfterRes.ppm", true);
    }

    cam.stopStreaming(true);
    cam.unInitMmapStreaming(true);
    cam.closeDevice(true);
  } catch (std::exception& e) {
    std::cout << "Error: " << e.what() << std::endl;
    return 2;
  }
  return 0;
}
