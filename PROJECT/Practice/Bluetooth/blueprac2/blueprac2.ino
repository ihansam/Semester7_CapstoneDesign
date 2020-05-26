#include <SoftwareSerial.h>
//시리얼통신 라이브러리 호출
int blueTx=3; //Tx (보내는핀 설정)at
int blueRx=2; //Rx (받는핀 설정)
SoftwareSerial mySerial(blueTx, blueRx);
//시리얼 통신을 위한 객체선언
void setup()
{
  Serial.begin(9600); //시리얼모니터
  mySerial.begin(9600); //블루투스 시리얼
}

unsigned long currT = 0;
unsigned long pastT = 0;
void loop()
{
  currT = millis();
  /*
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }
  */
/*
  if (currT - pastT > 1000) {
    mySerial.print("hello");
    //mySerial.print(currT);
    pastT = currT;
  }
*/
  mySerial.print(65);
  delay(3000);
  mySerial.print("4321");
  delay(3000);
  mySerial.print("github");
  delay(3000);
  mySerial.print("game");
  delay(10000);
}
