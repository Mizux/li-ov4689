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

On some heads, we have some timeout when trying to get images. problem dissapear after several seconds and few loops
(i.e. need to opn/close the device several time to get back the stream).

note: first 4 images are empty.
