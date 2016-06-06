# Description
Is it currently impossible to update camera resolution (i.e. S_FMT) once STREAM_OFF is call.  
We have too close/open the device again.  
note: first 4 images are empty due to an other issue (see **camera_queue**).

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
./dev/bin/camera_startup -d /dev/video0 -w 2560 -h 720
```

note: you can use *--verbose* if you want more trace

Expect no error.
Observed:
```
Test1 (setRes, setFps, start): START
Test1: DONE
Test2 (setFps, setRes, start): START
Test2: DONE
Test3 (stop, start): START
Test3: DONE
Test4 (stop, setRes, start): START
(ERROR) /dev/video-top: VIDIOC_S_FMT (5): Device or resource busy
Test4: FAIL
Test5 (stop, setFps, start): START
Test5: DONE
Test6 (stop, setFps, setRes, start): START
(ERROR) /dev/video-top: VIDIOC_S_FMT (5): Device or resource busy
Test6: FAIL
Test7 (stop, setRes, setFps, start): START
(ERROR) /dev/video-top: VIDIOC_S_FMT (5): Device or resource busy
Test7: FAIL
```

As we can, only tests which try to *setResolution()* after stream has been stop once will fail.
