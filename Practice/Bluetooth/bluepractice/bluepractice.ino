#include <SoftwareSerial.h>
#define RX_PIN 3          // HC의 TX에 연결
#define TX_PIN 2          // HC의 RX에 연결
#define SERIAL_BAUD 9600
#define HC_BAUD 9600


SoftwareSerial HCSerial(RX_PIN, TX_PIN);
 
void setup() {
  Serial.begin(SERIAL_BAUD);
  HCSerial.begin(HC_BAUD);
}

String myString = "";

void loop() {
  while(Serial.available())
  {
    char myChar = (char)Serial.read();
    myString+=myChar;
    delay(5);                           //수신 문자열 끊김 방지
  }

  if(!myString.equals(""))
  {
    Serial.print("input: " + myString);
    HCSerial.print(myString);
    myString="";
  }
}
