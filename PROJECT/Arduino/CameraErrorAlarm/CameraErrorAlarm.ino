// last update 2020.06.05 by Kidan Jin
#include <SoftwareSerial.h>     // 통신(블루투스)을 위한 라이브러리
const int bteTx = 3;            // TXD pin번호
const int bteRx=2;              // RxD pin번호
SoftwareSerial bteSerial(bteTx, bteRx);  // 통신 객체

void CameraErrorAlarm()          // 어플로 카메라 위치 오류 알람
{
  static int erroridx = 0;
  String instruction = "C";
  instruction += erroridx++;
  bteSerial.print(instruction);
  Serial.println(instruction);
}

void setup()
{
  Serial.begin(9600);
  bteSerial.begin(9600);
}

void loop()
{
  CameraErrorAlarm();            // 함수 호출
  delay(10000);
}
