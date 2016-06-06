//! @file
//! Test what happen camera is stop and restarted.
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
    std::cout << "Test1 (setRes, setFps, start): START" << std::endl;
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
    cam.unInitMmapStreaming();
    cam.closeDevice();
    std::cout << "Test1: DONE" << std::endl;
  } catch (std::exception& e) {
    std::cout << "Test1: FAIL" << std::endl;
  }

  try {
    std::cout << "Test2 (setFps, setRes, start): START" << std::endl;
    Camera cam(argc, argv);
    cam.openDevice();
    cam.setFrameRate();
    cam.setResolution();
    cam.initMmapStreaming();
    cam.startStreaming();
    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000 * (cam.bufferNb()+2) / cam.fps())); // Fill all buffer
    cam.flushBuffer();
    cam.writePPM("FpsBeforeRes.ppm");
    cam.stopStreaming();
    cam.unInitMmapStreaming();
    cam.closeDevice();
    std::cout << "Test2: DONE" << std::endl;
  } catch (std::exception& e) {
    std::cout << "Test2: FAIL" << std::endl;
  }

  try {
    std::cout << "Test3 (stop, start): START" << std::endl;
    Camera cam(argc, argv);
    cam.openDevice();
    cam.setFrameRate();
    cam.setResolution();
    cam.initMmapStreaming();
    cam.startStreaming();
    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000 * (cam.bufferNb()+2) / cam.fps())); // Fill all buffer
    cam.flushBuffer();
    // stop camera, then restart it.
    cam.stopStreaming();
    cam.unInitMmapStreaming();
    cam.initMmapStreaming();
    cam.startStreaming();
    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000 * (cam.bufferNb()+2) / cam.fps())); // Fill all buffer
    cam.flushBuffer();
    cam.writePPM("StopStart.ppm");
    cam.stopStreaming();
    cam.unInitMmapStreaming();
    cam.closeDevice();
    std::cout << "Test3: DONE" << std::endl;
  } catch (std::exception& e) {
    std::cout << "Test3: FAIL" << std::endl;
  }

  try {
    std::cout << "Test4 (stop, setRes, start): START" << std::endl;
    Camera cam(argc, argv);
    cam.openDevice();
    cam.setResolution();
    cam.setFrameRate();
    cam.initMmapStreaming();
    cam.startStreaming();
    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000 * (cam.bufferNb()+2) / cam.fps())); // Fill all buffer
    cam.flushBuffer();
    cam.stopStreaming();
    cam.unInitMmapStreaming();

    cam.setResolution();

    cam.initMmapStreaming();
    cam.startStreaming();
    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000 * (cam.bufferNb()+2) / cam.fps())); // Fill all buffer
    cam.flushBuffer();
    cam.writePPM("ResUpdate.ppm");
    cam.stopStreaming();
    cam.unInitMmapStreaming();
    cam.closeDevice();
    std::cout << "Test4: DONE" << std::endl;
  } catch (std::exception& e) {
    std::cout << "Test4: FAIL" << std::endl;
  }

  try {
    std::cout << "Test5 (stop, setFps, start): START" << std::endl;
    Camera cam(argc, argv);
    cam.openDevice();
    cam.setResolution();
    cam.setFrameRate();
    cam.initMmapStreaming();
    cam.startStreaming();
    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000 * (cam.bufferNb()+2) / cam.fps())); // Fill all buffer
    cam.flushBuffer();
    cam.stopStreaming();
    cam.unInitMmapStreaming();

    cam.setFrameRate();

    cam.initMmapStreaming();
    cam.startStreaming();
    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000 * (cam.bufferNb()+2) / cam.fps())); // Fill all buffer
    cam.flushBuffer();
    cam.writePPM("FpsUpdate.ppm");
    cam.stopStreaming();
    cam.unInitMmapStreaming();
    cam.closeDevice();
    std::cout << "Test5: DONE" << std::endl;
  } catch (std::exception& e) {
    std::cout << "Test5: FAIL" << std::endl;
  }

  try {
    std::cout << "Test6 (stop, setFps, setRes, start): START" << std::endl;
    Camera cam(argc, argv);
    cam.openDevice();
    cam.setResolution();
    cam.setFrameRate();
    cam.initMmapStreaming();
    cam.startStreaming();
    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000 * (cam.bufferNb()+2) / cam.fps())); // Fill all buffer
    cam.flushBuffer();
    cam.stopStreaming();
    cam.unInitMmapStreaming();

    cam.setFrameRate();
    cam.setResolution();

    cam.initMmapStreaming();
    cam.startStreaming();
    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000 * (cam.bufferNb()+2) / cam.fps())); // Fill all buffer
    cam.flushBuffer();
    cam.writePPM("FpsResUpdate.ppm");
    cam.stopStreaming();
    cam.unInitMmapStreaming();
    cam.closeDevice();
    std::cout << "Test6: DONE" << std::endl;
  } catch (std::exception& e) {
    std::cout << "Test6: FAIL" << std::endl;
  }

  try {
    std::cout << "Test7 (stop, setRes, setFps, start): START" << std::endl;
    Camera cam(argc, argv);
    cam.openDevice();
    cam.setResolution();
    cam.setFrameRate();
    cam.initMmapStreaming();
    cam.startStreaming();
    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000 * (cam.bufferNb()+2) / cam.fps())); // Fill all buffer
    cam.flushBuffer();
    cam.stopStreaming();
    cam.unInitMmapStreaming();

    cam.setResolution();
    cam.setFrameRate();

    cam.initMmapStreaming();
    cam.startStreaming();
    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000 * (cam.bufferNb()+2) / cam.fps())); // Fill all buffer
    cam.flushBuffer();
    cam.writePPM("ResFpsUpdate.ppm");
    cam.stopStreaming();
    cam.unInitMmapStreaming();
    cam.closeDevice();
    std::cout << "Test7: DONE" << std::endl;
  } catch (std::exception& e) {
    std::cout << "Test7: FAIL" << std::endl;
  }

  return 0;
}
