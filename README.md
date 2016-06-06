# Description
You can find here, programs to reproduce spotted issue (firmware OV580SpecificFW20151229.bin) for OV4689.  
When option *--dump* is used, images are converted to RGB and stored using ppm (P6) format and can be view by any image viewer (e.g. feh)

# LI-OV4689
Current known issue for stereo camera aka OV4689x2 -> OV580 -> USB 3.0

1. The camera timeout when trying to get the first image after a **STREAM_ON** if camera is over-exposed. (i.e. over exposure is an issue at startup **only**).  
Please see the  **[ov4689_full](ov4689_full)** readme for further details.

2. It seems that camera can't restart after a STREAM_OFF.  
Please see the  **[camera_startup](camera_startup)** readme for further details.

3. Usually first N images (with N equal to v4l2 queue buffer size) are empty.
Please see the  **[camera_queue](camera_queue)** readme for further details.

# LI-OV5640
Current known issue for 2D camera aka OV5640 -> CX3 -> USB 3.0

1. In manual exposure mode, range is too short (400) and start with an exposure too long.  
Please see the **[ov5640_exposure](ov5640_exposure)** readme for further details.

2. Sometime, UVC controls fails.  
Please see the **[ov5640_ext](ov5640_ext)** readme for further details.

3. Register state (e.g. flip) are lost when stream is stopped.  
Please see the **[ov5640_flip](ov5640_flip)** readme for further details.

4. It seems that camera can't restart after a STREAM_OFF.  
Please see the  **[camera_startup](camera_startup)** readme for further details.

5. Usually first N images (with N equal to v4l2 queue buffer size) are empty.
Please see the  **[camera_queue](camera_queue)** readme for further details.

# HowTo Build
## Dependencies
Project use CMake >= 3.2, C++11, and v4l2 API (i.e. programs are linux only).

To install new cmake version on *old* Ubuntu just retrieve a new version on ppa.
```sh
sudo add-apt-repository ppa:george-edison55/cmake-3.x -y
sudo apt-get update -qq
sudo apt-get install -qq cmake
```

## Pure CMake build
On Desktop
```sh
mkdir build && cd build
cmake ..
make
```
## Using qiBuild (Aldebaran cross-compilation for robot)
Build & Deploy:
```sh
qibuild configure -c juliette --release
qibuild make -c juliette
qibuild deploy -c juliette --url nao@ip.of.the.robot:~/foo
```
On robot:
```sh
ssh nao@ip.of.the.robot
./foo/bin/ov4689_full -d /dev/video-stereo -w 2560 -h 720 -fps 15
```

### Retrieve images
To retrieve images on Desktop:
```sh
rm *.ppm
scp nao@ip.of.the.robot:/home/nao/foo/bin/\*.ppm .
```

Display images:
```sh
feh *.ppm
```
