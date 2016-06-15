#include "camera.hpp"

#include <chrono>
#include <thread>

int
main(int argc, char* argv[]) {
  if (argc < 2) {
    getUsage(argv[0]);
    return 1;
  }

  try {
    CameraLIOV5640 cam(argc, argv);
    cam.openDevice();
    cam.setResolution(640, 480);
    cam.setFrameRate(30);
    cam.setFlip(1);
    cam.initMmapStreaming();
    cam.startStreaming();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000 / cam.fps()));
    cam.flushBuffer();
    std::this_thread::sleep_for(std::chrono::seconds(5));

    cam.setAutoExposure(true);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    cam.writePPM("1_ExposureAuto.ppm");

    cam.setAutoExposure(false);
    cam.setGain(20);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    cam.writePPM("2_Gain20.ppm");

    cam.setExposure(1);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    cam.writePPM("3_Exposure1.ppm");

    cam.setGain(1);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    cam.writePPM("4_Gain1.ppm");

    cam.setExposure(1);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    cam.writePPM("5_Exposure1.ppm");

    cam.stopStreaming();
    cam.unInitMmapStreaming();
    cam.closeDevice();
  } catch (std::exception& e) {
    std::cerr << "(ERROR) TEST FAIL: " << e.what() << std::endl;
    return 2;
  }
  std::cout << "(INFO) TEST PASS" << std::endl;
  return 0;
}
