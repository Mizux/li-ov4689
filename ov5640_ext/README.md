# Symptoms

Errors occurs randomly when querying UVC registers.  

We have reproduced this situation on Pepper robot (Linux 4.0.4) and on our desktop machine 
(Linux 4.4.5).

# Test description

This test will querry 100 times some control extension unit (read and write access). When a query 
fails, a log is printed.

# How to build
As any cmake project, it simple to build it.

```sh
$ mkdir build && cd build
$ cmake ..
$ make
```

# How to reproduce

Execute the test program with the video device to test:

```sh
$ ./ov5640_ext/ov5640_ext /dev/video0
(ERROR) Fails in loop 45: /dev/video0: UVC_GET_CUR 12 fails: Input/output error
(ERROR) Fails in loop 69: /dev/video0: UVC_GET_CUR 12 fails: Input/output error
(ERROR) Fails in loop 70: /dev/video0: UVC_SET_CUR 12 fails: Input/output error
(ERROR) Fails in loop 72: /dev/video0: UVC_SET_CUR 13 fails: Input/output error
(ERROR) Fails in loop 95: /dev/video0: UVC_SET_CUR 12 fails: Input/output error
$ ./ov5640_ext/ov5640_ext /dev/video0
(ERROR) Fails in loop 20: /dev/video0: UVC_GET_CUR 10 fails: Input/output error
(ERROR) Fails in loop 41: /dev/video0: UVC_GET_CUR 12 fails: Input/output error
```

Depending on the errors the kernel show broken pipe errors (-32) or connection timed out errors (-110) like:

```sh
$ dmesg | tail
[691095.497035] uvcvideo: Failed to query (SET_CUR) UVC control 10 on unit 3: -32 (exp. 2).
[691215.774805] uvcvideo: Failed to query (SET_CUR) UVC control 12 on unit 3: -32 (exp. 2).
[691237.283248] uvcvideo: Failed to query (SET_CUR) UVC control 12 on unit 3: -110 (exp. 2).
[691237.286855] uvcvideo: Failed to query (GET_CUR) UVC control 12 on unit 3: -71 (exp. 2).
[691245.476993] uvcvideo: Failed to query (SET_CUR) UVC control 10 on unit 3: -32 (exp. 2).
[691263.684537] uvcvideo: Failed to query (GET_CUR) UVC control 12 on unit 3: -32 (exp. 2).
[691314.602783] uvcvideo: Failed to query (SET_CUR) UVC control 12 on unit 3: -32 (exp. 2).
[691314.803909] uvcvideo: Failed to query (SET_CUR) UVC control 10 on unit 3: -32 (exp. 2).
[691315.000640] uvcvideo: Failed to query (GET_CUR) UVC control 13 on unit 3: -32 (exp. 2).
```

