# Description
On both cameras, first images returned by the kernel are empty.  
It seems related to the number of the v4l2 buffers.

Here the sequence diagram.  
![Alt text](http://g.gravizo.com/g?
@startuml;
autonumber;
actor User;
participant "Camera" as C;
group Test1;
User -> C: OpenDevice\(/dev/video0\);
activate C;
User -> C: SetResolution\(\);
User -> C: SetFramerate\(\);
User -> C: InitMmapStreaming\(3\);
User -> C: StartStreaming\(\);
C -> User: Write N images\(first1_X.ppm\);
User -> C: StopStreaming\(\);
User -> C: UnintMmapStreaming\(\);
User -> C: CloseDevice\(\);
destroy C;
end;
group Test2;
User -> C: OpenDevice\(/dev/video0\);
activate C;
User -> C: SetResolution\(\);
User -> C: SetFramerate\(\);
User -> C: InitMmapStreaming\(8\);
User -> C: StartStreaming\(\);
C -> User: Write N images\(first2_X.ppm\);
User -> C: StopStreaming\(\);
User -> C: UnintMmapStreaming\(\);
User -> C: CloseDevice\(\);
destroy C;
end;
group Test3;
User -> C: OpenDevice\(/dev/video0\);
activate C;
User -> C: SetResolution\(\);
User -> C: SetFramerate\(\);
User -> C: InitMmapStreaming\(8\);
User -> C: StartStreaming\(\);
User -> C: Flush\(\);
C -> User: Write N images\(first3_X.ppm\);
User -> C: StopStreaming\(\);
User -> C: UnintMmapStreaming\(\);
User -> C: CloseDevice\(\);
destroy C;
end;
group Test4;
User -> C: OpenDevice\(/dev/video0\);
activate C;
User -> C: SetResolution\(\);
User -> C: SetFramerate\(\);
User -> C: InitMmapStreaming\(8\);
User -> C: StartStreaming\(\);
User -> User: wait 1/fps s;
User -> C: Flush\(\);
C -> User: Write N images\(first4_X.ppm\);
User -> C: StopStreaming\(\);
User -> C: UnintMmapStreaming\(\);
User -> C: CloseDevice\(\);
destroy C;
end;
@enduml
)

On test 1: first 3 images are empty.  
On test 2: first 8 images are empty.  
On test 3: first 8 images are empty even if the *flush* is performed.  
On test 4: Once the flush is done with a small delay after **StartStreaming()** command is done, all images are OK (modulo AWB/AEC stabilization...)

# Testing
to test with ov4689 stereo camera you can run the command:
```sh
./camera_queue -d /dev/video0 -w 2560 -h 720 --verbose
```

Observed:
```
Test1: START
(VERBOSE) /dev/video-stereo: open device
(VERBOSE) /dev/video-stereo: set resolution to 2560x720
(VERBOSE) /dev/video-stereo: set framerate to 15
(VERBOSE) /dev/video-stereo: allocating memory (3 buffers)
(VERBOSE) /dev/video-stereo: start streaming
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   0seq timestamp:    0s,   0ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   0seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   0seq timestamp:    0s,   0ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   0seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   0seq timestamp:    0s,   0ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   0seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   0seq timestamp: 10218s, 419ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   0seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   1seq timestamp: 10218s, 486ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   1seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   2seq timestamp: 10218s, 552ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   2seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   3seq timestamp: 10218s, 619ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   3seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   4seq timestamp: 10218s, 686ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   4seq
(VERBOSE) /dev/video-stereo: stop streaming
(VERBOSE) /dev/video-stereo: deallocating memory
(VERBOSE) /dev/video-stereo: closing camera
Test1: DONE
Test2: START
(VERBOSE) /dev/video-stereo: open device
(VERBOSE) /dev/video-stereo: set resolution to 2560x720
(VERBOSE) /dev/video-stereo: set framerate to 15
(VERBOSE) /dev/video-stereo: allocating memory (8 buffers)
(VERBOSE) /dev/video-stereo: start streaming
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   0seq timestamp:    0s,   0ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   0seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   0seq timestamp:    0s,   0ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   0seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   0seq timestamp:    0s,   0ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   0seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   0seq timestamp:    0s,   0ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   0seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   0seq timestamp:    0s,   0ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   0seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   0seq timestamp:    0s,   0ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   0seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   0seq timestamp:    0s,   0ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   0seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   0seq timestamp:    0s,   0ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   0seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   0seq timestamp: 10219s,  19ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   0seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   1seq timestamp: 10219s,  86ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   1seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   2seq timestamp: 10219s, 152ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   2seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   3seq timestamp: 10219s, 219ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   3seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   4seq timestamp: 10219s, 286ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   4seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   5seq timestamp: 10219s, 352ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   5seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   6seq timestamp: 10219s, 419ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   6seq
(VERBOSE) /dev/video-stereo: stop streaming
(VERBOSE) /dev/video-stereo: deallocating memory
(VERBOSE) /dev/video-stereo: closing camera
Test2: DONE
Test3: START
(VERBOSE) /dev/video-stereo: open device
(VERBOSE) /dev/video-stereo: set resolution to 2560x720
(VERBOSE) /dev/video-stereo: set framerate to 15
(VERBOSE) /dev/video-stereo: allocating memory (8 buffers)
(VERBOSE) /dev/video-stereo: start streaming
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   0seq timestamp:    0s,   0ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   0seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   0seq timestamp:    0s,   0ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   0seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   0seq timestamp:    0s,   0ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   0seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   0seq timestamp:    0s,   0ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   0seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   0seq timestamp:    0s,   0ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   0seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   0seq timestamp:    0s,   0ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   0seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   0seq timestamp:    0s,   0ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   0seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   0seq timestamp:    0s,   0ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   0seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   0seq timestamp: 10219s, 752ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   0seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   1seq timestamp: 10219s, 819ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   1seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   2seq timestamp: 10219s, 886ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   2seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   3seq timestamp: 10219s, 952ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   3seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   4seq timestamp: 10220s,  19ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   4seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   5seq timestamp: 10220s,  86ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   5seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   6seq timestamp: 10220s, 152ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   6seq
(VERBOSE) /dev/video-stereo: stop streaming
(VERBOSE) /dev/video-stereo: deallocating memory
(VERBOSE) /dev/video-stereo: closing camera
Test3: DONE
Test4: START
(VERBOSE) /dev/video-stereo: open device
(VERBOSE) /dev/video-stereo: set resolution to 2560x720
(VERBOSE) /dev/video-stereo: set framerate to 15
(VERBOSE) /dev/video-stereo: allocating memory (8 buffers)
(VERBOSE) /dev/video-stereo: start streaming
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock and release buffer,  timecode:   0seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   0seq timestamp: 10220s, 486ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   0seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   1seq timestamp: 10220s, 552ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   1seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   2seq timestamp: 10220s, 619ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   2seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   3seq timestamp: 10220s, 686ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   3seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   4seq timestamp: 10220s, 752ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   4seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   5seq timestamp: 10220s, 819ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   5seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   6seq timestamp: 10220s, 886ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   6seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   7seq timestamp: 10220s, 952ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   7seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   8seq timestamp: 10221s,  19ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   8seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:   9seq timestamp: 10221s,  86ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:   9seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:  10seq timestamp: 10221s, 152ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:  10seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:  11seq timestamp: 10221s, 219ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:  11seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:  12seq timestamp: 10221s, 286ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:  12seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:  13seq timestamp: 10221s, 352ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:  13seq
(VERBOSE) /dev/video-stereo: lock buffer, timecode:  14seq timestamp: 10221s, 419ms
(VERBOSE) /dev/video-stereo: release buffer, timecode:  14seq
(VERBOSE) /dev/video-stereo: stop streaming
(VERBOSE) /dev/video-stereo: deallocating memory
(VERBOSE) /dev/video-stereo: closing camera
Test4: DONE
```

As you can see, first images have the same seq code and timestamp (null).
