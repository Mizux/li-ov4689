//! @file
//! Try to set resolution (only) once stream has been stop.
#include <camera.hpp>

#include <chrono>
#include <iostream>
#include <thread>

int
main(int argc, char* argv[]) {
  if (argc < 2) {
    getUsage(argv[0]);
    return 1;
  }

  std::cout << "Test1: Start/Stop/Start" << std::endl;
  try {
    CameraLIOV5640 cam(argc, argv);
    cam.openDevice();
    cam.setResolution();
    cam.setFrameRate();

    // Start
    cam.initMmapStreaming();
    cam.startStreaming();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000 / cam.fps()));
    cam.flushBuffer();
    // Shot
    cam.writePPM("test1_start.ppm");
    // Stop
    cam.stopStreaming();
    cam.unInitMmapStreaming();
    // Start
    cam.initMmapStreaming();
    cam.startStreaming();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000 / cam.fps()));
    cam.flushBuffer();
    // Shot
    cam.writePPM("test1_start_stop_start.ppm");

    cam.stopStreaming();
    cam.unInitMmapStreaming();
    cam.closeDevice();
  } catch (std::exception& e) {
    std::cerr << "(ERROR) " << e.what() << std::endl;
    return 2;
  }

  std::cout << "Test2: Start/Stop/S_FMT/Start" << std::endl;
  try {
    CameraLIOV5640 cam(argc, argv);
    cam.openDevice();
    cam.setResolution();
    cam.setFrameRate();

    // Start
    cam.initMmapStreaming();
    cam.startStreaming();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000 / cam.fps()));
    cam.flushBuffer();
    // Shot
    cam.writePPM("test2_start.ppm");
    // Stop
    cam.stopStreaming();
    cam.unInitMmapStreaming();
    // S_FMT
    cam.setResolution();
    // Start
    cam.initMmapStreaming();
    cam.startStreaming();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000 / cam.fps()));
    cam.flushBuffer();
    // Shot
    cam.writePPM("test2_start_stop_sfmt_start.ppm");

    cam.stopStreaming();
    cam.unInitMmapStreaming();
    cam.closeDevice();
  } catch (std::exception& e) {
    std::cerr << "(ERROR) " << e.what() << std::endl;
    return 2;
  }

  return 0;
}
