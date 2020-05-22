// last update 2020.05.22 by Kidan Jin
#include <SoftwareSerial.h>     // 통신(블루투스)을 위한 라이브러리
const int bteTx = 3;            // TXD pin번호
const int bteRx=2;              // RxD pin번호
SoftwareSerial bteSerial(bteTx, bteRx);  // 통신 객체

void StretchingAlarm()          // 어플로 스트레칭 하라는 instruction을 전달하는 함수
{
  int recommendNUM = random(1,6);
  String instruction = "S";
  instruction += recommendNUM;
  bteSerial.print(instruction); // S1 ~ S5라는 instruction을 랜덤으로 전달
}

void setup()
{
  Serial.begin(9600);
  bteSerial.begin(9600);
}

void loop()
{
  StretchingAlarm();            // 함수 호출
  delay(10000);
}
