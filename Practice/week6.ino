const int LEDcontrol = 10;
const int LIGHTsensor = 0;

  void setup()
{
  Serial.begin(9600);
  pinMode(LEDcontrol, OUTPUT);
}

void loop()
{
  int a0 = analogRead(LIGHTsensor);
  Serial.print(a0);
  int led = map(a0, 6, 679, 255, 0);
  Serial.print("  ");
  Serial.println(led);
  analogWrite(LEDcontrol, led);
  delay(500);
}