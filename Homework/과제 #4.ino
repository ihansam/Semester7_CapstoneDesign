const int LEDcontrol = 10;  // LED 연결 pin number
const int DIVIDER = 1;      // 분압기 연결 pin number

void setup()
{
  Serial.begin(9600);
  pinMode(LEDcontrol, OUTPUT);
}

void loop()
{
  int v = analogRead(DIVIDER);        // 분압기에 걸리는 전압 (0 ~ 1023)
  int led = map(v, 0, 1023, 0, 255);  // 분압기 전압을 0 ~ 255 범위의 LED 밝기로 변환
  analogWrite(LEDcontrol, led);       // LED 제어
  Serial.println(led);                // 모니터에 LED 밝기 표시
  delay(1000);
}