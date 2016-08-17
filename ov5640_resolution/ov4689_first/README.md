
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
User -> User: wait 1/fps s
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
On test 3: first 8 images are empty even if the flush is performed.
On test 4: Once the flush is done with a small delay after @Start()@ command is done, all images are OK (modulo AWB/AEC stabilization...)
