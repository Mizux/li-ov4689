
Here the sequence diagram.
![Alt text](http://g.gravizo.com/g?
@startuml;
autonumber;
actor User;
participant "Camera" as C;
User -> C: OpenDevice\(/dev/video0\);
activate C;
User -> C: SetResolution\(\);
User -> C: SetFramerate\(\);
group Test1;
User -> C: SetFlip\(1\);
note right: Images will be flipped;
User -> C: InitMmapStreaming\(\);
User -> C: StartStreaming\(\);
C -> User: Write image\(flip1.ppm\);
User -> C: StopStreaming\(\);
User -> C: UnintMmapStreaming\(\);
end;
group Test2;
User -> C: InitMmapStreaming\(\);
User -> C: StartStreaming\(\);
C -> User: Write image\(flip2.ppm\);
note right: FLIP IS LOST !!!!;
User -> C: StopStreaming\(\);
User -> C: UnintMmapStreaming\(\);
end;
group Test3;
User -> C: SetFlip\(1\);
User -> C: InitMmapStreaming\(\);
User -> C: StartStreaming\(\);
C -> User: Write image\(flip3.ppm\);
User -> C: StopStreaming\(\);
User -> C: UnintMmapStreaming\(\);
end;
User -> C: CloseDevice\(\);
destroy C;
@enduml
)

note: Please also notice first images (~4/5) are weird, before having correct flip images.
