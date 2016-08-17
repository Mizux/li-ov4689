#include "camera.hpp"

#include <thread>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <sstream>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "usage: " << argv[0] << " /dev/video0 [width height [fps]]" << std::endl;
    return 1;
  }

  int width = argc >= 4 ? std::atoi(argv[2]): 640;
  int height = argc >= 4 ? std::atoi(argv[3]): 480;
  int fps = argc >= 5 ? std::stoi(argv[4]): 30;

  try {
    CameraLIOV5640 cam;
    cam.openDevice(argv[1], true);
    cam.setResolution(width, height, true);
    cam.setFrameRate(fps, true);
    cam.setFlip(1, true);

    {
      std::cout << "Test1: START" << std::endl;
      cam.initMmapStreaming(3, true);
      cam.startStreaming(true);
      std::vector<std::unique_ptr<std::uint8_t>> buffers;
      for (int i = 0; i < 50; ++i) {
        buffers.push_back(cam.getImage(true));
      }
      int i=0;
      for (auto const& it: buffers) {
        std::stringstream filename;
        filename << "Flip1_BeforeStop" << std::setw(3) << std::setfill('0') << i++
                 << ".ppm";
        std::cout << filename.str() << ": ";
        Camera::writeBuffer2PPM(filename.str(), width, height, it.get(), true);
      }
      cam.stopStreaming(true);
      cam.unInitMmapStreaming(true);
      std::cout << "Test1: DONE" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(4));
    {
      std::cout << "Test2: START" << std::endl;
      cam.initMmapStreaming(3, true);
      cam.startStreaming(true);
      std::vector<std::unique_ptr<std::uint8_t>> buffers;
      for (int i = 0; i < 50; ++i) {
        buffers.push_back(cam.getImage(true));
      }
      int i=0;
      for (auto const& it: buffers) {
        std::stringstream filename;
        filename << "Flip2_AfterStop" << std::setw(3) << std::setfill('0') << i++
                 << ".ppm";
        std::cout << filename.str() << ": ";
        Camera::writeBuffer2PPM(filename.str(), width, height, it.get(), true);
      }
      cam.stopStreaming(true);
      cam.unInitMmapStreaming(true);
      std::cout << "Test2: DONE" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(4));
    {
      std::cout << "Test3: START" << std::endl;
      cam.setFlip(1, true);
      cam.initMmapStreaming(3, true);
      cam.startStreaming(true);
      std::vector<std::unique_ptr<std::uint8_t>> buffers;
      for (int i = 0; i < 50; ++i) {
        buffers.push_back(cam.getImage(true));
      }
      int i=0;
      for (auto const& it: buffers) {
        std::stringstream filename;
        filename << "Flip3_FlipAgain" << std::setw(3) << std::setfill('0') << i++
                 << ".ppm";
        std::cout << filename.str() << ": ";
        Camera::writeBuffer2PPM(filename.str(), width, height, it.get(), true);
      }
      cam.stopStreaming(true);
      cam.unInitMmapStreaming(true);
      std::cout << "Test3: DONE" << std::endl;
    }
    cam.closeDevice(true);
  } catch (std::exception& e) {
    std::cout << "Error: " << e.what() << std::endl;
    return 2;
  }
  return 0;
}
