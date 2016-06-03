//! @file
//! Test if Camera OV5640 keep flip orientation when stream is stop (i.e. STREAM_OFF)
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
    CameraLIOV5640 cam(argc, argv);
    cam.openDevice();
    cam.setResolution();
    cam.setFrameRate();
    cam.setFlip(1);
    {
      std::cout << "(INFO) Test1: START (set flip before stream start)" << std::endl;
      cam.initMmapStreaming();
      cam.startStreaming();
      std::vector<std::unique_ptr<std::uint8_t>> buffers;
      for (int i = 0; i < 50; ++i) {
        buffers.push_back(cam.getImage());
      }
      int i=0;
      for (auto const& it: buffers) {
        std::stringstream filename;
        filename << "Flip1_BeforeStop" << std::setw(3) << std::setfill('0') << i++
                 << ".ppm";
        std::cout << filename.str() << ": ";
        cam.writeBuffer2PPM(filename.str(), cam.width(), cam.height(), it.get());
      }
      cam.stopStreaming();
      cam.unInitMmapStreaming();
      std::cout << "(INFO) Test1: DONE" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(4));
    {
      std::cout << "(INFO) Test2: START (don't set flip before restart)" << std::endl;
      cam.initMmapStreaming();
      cam.startStreaming();
      std::vector<std::unique_ptr<std::uint8_t>> buffers;
      for (int i = 0; i < 50; ++i) {
        buffers.push_back(cam.getImage());
      }
      int i=0;
      for (auto const& it: buffers) {
        std::stringstream filename;
        filename << "Flip2_AfterStop" << std::setw(3) << std::setfill('0') << i++
                 << ".ppm";
        std::cout << filename.str() << ": ";
        cam.writeBuffer2PPM(filename.str(), cam.width(), cam.height(), it.get());
      }
      cam.stopStreaming();
      cam.unInitMmapStreaming();
      std::cout << "(INFO) Test2: DONE" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(4));
    {
      std::cout << "(INFO) Test3: START (set flip before restart)" << std::endl;
      cam.setFlip(1);
      cam.initMmapStreaming();
      cam.startStreaming();
      std::vector<std::unique_ptr<std::uint8_t>> buffers;
      for (int i = 0; i < 50; ++i) {
        buffers.push_back(cam.getImage());
      }
      int i=0;
      for (auto const& it: buffers) {
        std::stringstream filename;
        filename << "Flip3_FlipAgain" << std::setw(3) << std::setfill('0') << i++
                 << ".ppm";
        std::cout << filename.str() << ": ";
        cam.writeBuffer2PPM(filename.str(), cam.width(), cam.height(), it.get());
      }
      cam.stopStreaming();
      cam.unInitMmapStreaming();
      std::cout << "(INFO) Test3: DONE" << std::endl;
    }
    cam.closeDevice();
  } catch (std::exception& e) {
    std::cerr << "(ERROR) " << e.what() << std::endl;
    return 2;
  }
  return 0;
}
