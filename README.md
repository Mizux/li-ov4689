# Description
You can find here, programs to reproduce spotted issue (firmware OV580SpecificFW20151229.bin) for OV4689.  
When option *--dump* is used, images are converted to RGB and stored using ppm (P6) format and can be view by any image viewer (e.g. feh)

# LI-OV4689
Current known issue for stereo camera aka OV4689x2 -> OV580 -> USB 3.0

1) The camera timeout when trying to get the first image after a **STREAM_ON**
in front of a bright scene (i.e. over exposure seems to be an issue at startup).  
note: can be test with:
```
while true; do ./dev/bin/ov4689_full -d /dev/video0 -w 2560 -h 720 -fps 15 -loop 25; done
```
note: you can add *--verbose* option if you want more logs.  

2) Could dump images to show image corruption.
note: can be test with **ov4689_images** program (usually 3-5% on a run of 100 imgs !)

4) It seems that *S_FMT* and *S_PARM* are order dependent. Thus setFramerate() must be done after setResolution().  
note: can be test with **ov4689_order** program

# LI-OV5640
Current known issue for 2D camera aka OV5640 -> CX3 -> USB 3.0

1) In manual exposure mode, range is too short (400) and start with a exposure too long.  
Please see the **ov5640_exposure** readme for further details.

2) Register state (e.g. flip) are lost when stream is stopped.  
note: can be test with **ov5640_flip** program

3) We can't change resolution once the stream is stopped, you have to close and reopen the device. -> it seems the firmware do ugly things in the stop streaming (which should be done in close function).  
note: can be test with **ov5640_resolution** program

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
