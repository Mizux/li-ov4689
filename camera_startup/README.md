# Description
Is it currently impossible to restart camera once STREAM_OFF is call.
We have too close/open the device again.
note: first 4 images are empty due to an other issue (see **camera_first**).

Here the sequence diagram.  
![Alt text](http://g.gravizo.com/g?
@startuml;
autonumber;
actor Program;
participant "Camera" as C;
group Test1;
Program -> C: OpenDevice\(/dev/video0\);
activate C;
Program -> C: SetResolution\(2560, 720\);
Program -> C: SetFramerate\(15\);
Program -> C: InitMmapStreaming\(4\);
Program -> C: StartStreaming\(\);
C -> Program: writePPM\(FpsAfterRes.ppm\);
Program -> C: StopStreaming\(\);
Program -> C: UninitMmapStreaming\(\);
Program -> C: CloseDevice\(\);
destroy C;
end;
@enduml
)

![Alt text](http://g.gravizo.com/g?
@startuml;
autonumber;
actor Program;
participant "Camera" as C;
group Test2;
Program -> C: OpenDevice\(/dev/video0\);
activate C;
Program -> C: SetFramerate\(15\);
Program -> C: SetResolution\(2560, 720\);
Program -> C: InitMmapStreaming\(4\);
Program -> C: StartStreaming\(\);
C -> Program: writePPM\(FpsBeforeRes.ppm\);
Program -> C: StopStreaming\(\);
Program -> C: UninitMmapStreaming\(\);
Program -> C: CloseDevice\(\);
destroy C;
end;
@enduml
)

![Alt text](http://g.gravizo.com/g?
@startuml;
autonumber;
actor Program;
participant "Camera" as C;
group Test3;
Program -> C: OpenDevice\(/dev/video0\);
activate C;
Program -> C: SetResolution\(2560, 720\);
Program -> C: SetFramerate\(15\);
Program -> C: InitMmapStreaming\(4\);
Program -> C: StartStreaming\(\);
Program -> C: StopStreaming\(\);
Program -> C: UninitMmapStreaming\(\);
Program -> C: InitMmapStreaming\(4\);
Program -> C: StartStreaming\(\);
C -> Program: writePPM\(StopStart.ppm\);
Program -> C: StopStreaming\(\);
Program -> C: UninitMmapStreaming\(\);
Program -> C: CloseDevice\(\);
destroy C;
end;
@enduml
)

![Alt text](http://g.gravizo.com/g?
@startuml;
autonumber;
actor Program;
participant "Camera" as C;
group Test4;
Program -> C: OpenDevice\(/dev/video0\);
activate C;
Program -> C: SetResolution\(2560, 720\);
Program -> C: SetFramerate\(15\);
Program -> C: InitMmapStreaming\(4\);
Program -> C: StartStreaming\(\);
Program -> C: StopStreaming\(\);
Program -> C: UninitMmapStreaming\(\);
Program -> C: SetResolution\(2560, 720\);
Program -> C: InitMmapStreaming\(4\);
Program -> C: StartStreaming\(\);
C -> Program: writePPM\(ResUpdate.ppm\);
Program -> C: StopStreaming\(\);
Program -> C: UninitMmapStreaming\(\);
Program -> C: CloseDevice\(\);
destroy C;
end;
@enduml
)

![Alt text](http://g.gravizo.com/g?
@startuml;
autonumber;
actor Program;
participant "Camera" as C;
group Test5;
Program -> C: OpenDevice\(/dev/video0\);
activate C;
Program -> C: SetResolution\(2560, 720\);
Program -> C: SetFramerate\(15\);
Program -> C: InitMmapStreaming\(4\);
Program -> C: StartStreaming\(\);
Program -> C: StopStreaming\(\);
Program -> C: UninitMmapStreaming\(\);
Program -> C: SetFramerate\(15\);
Program -> C: InitMmapStreaming\(4\);
Program -> C: StartStreaming\(\);
C -> Program: writePPM\(FpsUpdate.ppm\);
Program -> C: StopStreaming\(\);
Program -> C: UninitMmapStreaming\(\);
Program -> C: CloseDevice\(\);
destroy C;
end;
@enduml
)

![Alt text](http://g.gravizo.com/g?
@startuml;
autonumber;
actor Program;
participant "Camera" as C;
group Test6;
Program -> C: OpenDevice\(/dev/video0\);
activate C;
Program -> C: SetResolution\(2560, 720\);
Program -> C: SetFramerate\(15\);
Program -> C: InitMmapStreaming\(4\);
Program -> C: StartStreaming\(\);
Program -> C: StopStreaming\(\);
Program -> C: UninitMmapStreaming\(\);
Program -> C: SetFramerate\(15\);
Program -> C: SetResolution\(2560, 720\);
Program -> C: InitMmapStreaming\(4\);
Program -> C: StartStreaming\(\);
C -> Program: writePPM\(FpsResUpdate.ppm\);
Program -> C: StopStreaming\(\);
Program -> C: UninitMmapStreaming\(\);
Program -> C: CloseDevice\(\);
destroy C;
end;
@enduml
)

![Alt text](http://g.gravizo.com/g?
@startuml;
autonumber;
actor Program;
participant "Camera" as C;
group Test7;
Program -> C: OpenDevice\(/dev/video0\);
activate C;
Program -> C: SetResolution\(2560, 720\);
Program -> C: SetFramerate\(15\);
Program -> C: InitMmapStreaming\(4\);
Program -> C: StartStreaming\(\);
Program -> C: StopStreaming\(\);
Program -> C: UninitMmapStreaming\(\);
Program -> C: SetResolution\(2560, 720\);
Program -> C: SetFramerate\(15\);
Program -> C: InitMmapStreaming\(4\);
Program -> C: StartStreaming\(\);
C -> Program: writePPM\(ResFpsUpdate.ppm\);
Program -> C: StopStreaming\(\);
Program -> C: UninitMmapStreaming\(\);
Program -> C: CloseDevice\(\);
destroy C;
end;
@enduml
)

# Testing
You can test for ov4689 with:
```sh
./dev/bin/camera_order -d /dev/video0 -w 2560 -h 720 --verbose
```

Expect no error.
Observed:
```
Test1 (setRes, setFps, start): START
(VERBOSE) /dev/video-stereo: open device
(VERBOSE) /dev/video-stereo: set resolution to 2560x720
(VERBOSE) /dev/video-stereo: set framerate to 15
(VERBOSE) /dev/video-stereo: allocating memory (4 buffers)
(VERBOSE) /dev/video-stereo: start streaming
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   3seq timestamp: 6313s, 704ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   3seq
(VERBOSE) /dev/video-stereo: write image FpsAfterRes.ppm
(VERBOSE) /dev/video-stereo: stop streaming
(VERBOSE) /dev/video-stereo: deallocating memory
(VERBOSE) /dev/video-stereo: closing camera
Test1: DONE
Test2 (setFps, setRes, start): START
(VERBOSE) /dev/video-stereo: open device
(VERBOSE) /dev/video-stereo: set framerate to 15
(VERBOSE) /dev/video-stereo: set resolution to 2560x720
(VERBOSE) /dev/video-stereo: allocating memory (4 buffers)
(VERBOSE) /dev/video-stereo: start streaming
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:  10seq timestamp: 6315s, 319ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:  10seq
(VERBOSE) /dev/video-stereo: write image FpsBeforeRes.ppm
(VERBOSE) /dev/video-stereo: stop streaming
(VERBOSE) /dev/video-stereo: deallocating memory
(VERBOSE) /dev/video-stereo: closing camera
Test2: DONE
Test3 (stop, start): START
(VERBOSE) /dev/video-stereo: open device
(VERBOSE) /dev/video-stereo: set framerate to 15
(VERBOSE) /dev/video-stereo: set resolution to 2560x720
(VERBOSE) /dev/video-stereo: allocating memory (4 buffers)
(VERBOSE) /dev/video-stereo: start streaming
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: stop streaming
(VERBOSE) /dev/video-stereo: start streaming
(ERROR) /dev/video-stereo: VIDIOC_DQBUF (17): Resource temporarily unavailable
(VERBOSE) /dev/video-stereo: stop streaming
(VERBOSE) /dev/video-stereo: deallocating memory
(VERBOSE) /dev/video-stereo: closing camera
Test3: FAIL
Test4 (stop, setRes, start): START
(VERBOSE) /dev/video-stereo: open device
(VERBOSE) /dev/video-stereo: set resolution to 2560x720
(VERBOSE) /dev/video-stereo: set framerate to 15
(VERBOSE) /dev/video-stereo: allocating memory (4 buffers)
(VERBOSE) /dev/video-stereo: start streaming
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: stop streaming
(VERBOSE) /dev/video-stereo: set resolution to 2560x720
(ERROR) /dev/video-stereo: VIDIOC_S_FMT (5): Device or resource busy
(VERBOSE) /dev/video-stereo: stop streaming
(VERBOSE) /dev/video-stereo: deallocating memory
(VERBOSE) /dev/video-stereo: closing camera
Test4: FAIL
Test5 (stop, setFps, start): START
(VERBOSE) /dev/video-stereo: open device
(VERBOSE) /dev/video-stereo: set resolution to 2560x720
(VERBOSE) /dev/video-stereo: set framerate to 15
(VERBOSE) /dev/video-stereo: allocating memory (4 buffers)
(VERBOSE) /dev/video-stereo: start streaming
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: stop streaming
(VERBOSE) /dev/video-stereo: set framerate to 15
(VERBOSE) /dev/video-stereo: start streaming
(ERROR) /dev/video-stereo: VIDIOC_DQBUF (17): Resource temporarily unavailable
(VERBOSE) /dev/video-stereo: stop streaming
(VERBOSE) /dev/video-stereo: deallocating memory
(VERBOSE) /dev/video-stereo: closing camera
Test5: FAIL
Test6 (stop, setFps, setRes, start): START
(VERBOSE) /dev/video-stereo: open device
(VERBOSE) /dev/video-stereo: set resolution to 2560x720
(VERBOSE) /dev/video-stereo: set framerate to 15
(VERBOSE) /dev/video-stereo: allocating memory (4 buffers)
(VERBOSE) /dev/video-stereo: start streaming
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: stop streaming
(VERBOSE) /dev/video-stereo: set framerate to 15
(VERBOSE) /dev/video-stereo: set resolution to 2560x720
(ERROR) /dev/video-stereo: VIDIOC_S_FMT (5): Device or resource busy
(VERBOSE) /dev/video-stereo: stop streaming
(VERBOSE) /dev/video-stereo: deallocating memory
(VERBOSE) /dev/video-stereo: closing camera
Test6: FAIL
Test7 (stop, setRes, setFps, start): START
(VERBOSE) /dev/video-stereo: open device
(VERBOSE) /dev/video-stereo: set resolution to 2560x720
(VERBOSE) /dev/video-stereo: set framerate to 15
(VERBOSE) /dev/video-stereo: allocating memory (4 buffers)
(VERBOSE) /dev/video-stereo: start streaming
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: stop streaming
(VERBOSE) /dev/video-stereo: set resolution to 2560x720
(ERROR) /dev/video-stereo: VIDIOC_S_FMT (5): Device or resource busy
(VERBOSE) /dev/video-stereo: stop streaming
(VERBOSE) /dev/video-stereo: deallocating memory
(VERBOSE) /dev/video-stereo: closing camera
Test7: FAIL
```

