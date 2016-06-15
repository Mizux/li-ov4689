# Description
You can find here, a program to flash a new firmware for the leopard stereo camera LI-OV4689 aka OV4689x2 -> OV580 -> USB 3.0.  

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
On Desktop (i.e. native build):
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
```

# HowTo Test
## On Desktop
After project has been build, you can test it using:
```sh
./build/bin/flash-ov580 fwcmp OV580SpecificFW20151229.bin
```

## On Robot
After having deployed program on robot:

```sh
qibuild deploy -c juliette --url nao@ip.of.the.robot:~/foo
```

You can run it, using:
```sh
ssh nao@ip.of.the.robot
./foo/bin/flash-ov580 fwcmp OV580SpecificFW20151229.bin
```


