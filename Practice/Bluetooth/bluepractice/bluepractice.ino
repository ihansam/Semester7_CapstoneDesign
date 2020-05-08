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
 
void loop() {
  if(HCSerial.available()) {  // HC로 데이터가 들어올 때
    HCFunction();
  }
  if(Serial.available()) {    // 모니터로 데이터가 들어올 때
    SerialFunction();
  }
}
 
void HCFunction() {
  /*
   * 시리얼 모니터에 출력
   */
  Serial.write(HCSerial.read());
}
 
void SerialFunction() {
  /*
   * 블루투스 모듈에 출력
   */
  HCSerial.write(Serial.read());
}
