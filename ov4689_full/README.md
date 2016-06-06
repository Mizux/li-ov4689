# Description
We have some timeout when trying to get images, if camera is overexposed at startup.
note: first 4 images are empty due to an other issue (see **ov4689_first**).

Here the sequence diagram.  
![Alt text](http://g.gravizo.com/g?
@startuml;
autonumber;
actor Program;
participant "Camera" as C;
group Test Loop \(default 100\);
Program -> C: OpenDevice\(/dev/video0\);
activate C;
Program -> C: SetResolution\(2560, 720\);
Program -> C: SetFramerate\(15\);
Program -> C: InitMmapStreaming\(4\);
Program -> C: StartStreaming\(\);
C -> Program: GetImages\(15\);
Program -> C: StopStreaming\(\);
Program -> C: UninitMmapStreaming\(\);
Program -> C: CloseDevice\(\);
destroy C;
end;
@enduml
)

# Testing
You can test with:
```sh
while true; do ./dev/bin/ov4689_full -d /dev/video0 -w 2560 -h 720 -fps 15 -loop 25; done
```

Expected
```
(INFO) TEST PASS (0 fails on 25 runs)
(INFO): 2016-06-06 11:11:35.617706949+02:00
```

note: you can add *--verbose* option if you want more logs.
