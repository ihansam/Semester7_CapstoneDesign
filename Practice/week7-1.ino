// 초음파 센서가 감지한 거리가 가까울 수록 LED 밝게
void setup()
{
  Serial.begin(9600);
  pinMode(10, OUTPUT);  // LED pin
  pinMode(12, OUTPUT);  // triger pin
  pinMode(11, INPUT);   // eco pin
}

void loop()
{
  int LEDlight;
  int distance = measure();
  if(distance>100) LEDlight = 0;
  else if(distance > 20) LEDlight = map(distance, 100, 20, 50, 250);
  else LEDlight = 255;
  analogWrite(10, LEDlight);
  Serial.print(distance);
  Serial.print("cm : ");
  Serial.println(LEDlight);
  delay(500);
}

int measure()
{
  int duration, distance;
  
  digitalWrite(12, HIGH);
  delayMicroseconds(10);
  digitalWrite(12, LOW);

  duration = pulseIn(11, HIGH);
  distance = duration/29/2;

  return distance;
}