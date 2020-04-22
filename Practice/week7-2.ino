// 가변저항을 돌려서 RGB LED 색 제어
const int Rpin = 6;
const int Gpin = 11;
const int Bpin = 10;
const int Pmeter = A1;
const int Button = 9;
int pressed = 1;
int mode = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(Rpin, OUTPUT);  // LED pin
  pinMode(Gpin, OUTPUT);  // LED pin
  pinMode(Bpin, OUTPUT);  // LED pin
  pinMode(Button, INPUT_PULLUP);
}

void loop()
{
  int PRESS = digitalRead(Button);
  if (PRESS == 0) pressed = 0;
  else if (pressed == 0){
    pressed = 1;
    mode = (++mode)%3;
  }

  int potential = map(analogRead(Pmeter), 0, 1023, 0, 255);
  if (mode == 0) analogWrite(Rpin, potential);
  else if (mode == 1) analogWrite(Gpin, potential);
  else analogWrite(Bpin, potential);

  Serial.print(mode);
  Serial.print(": ");
  Serial.println(potential);
  delay(100);
}
