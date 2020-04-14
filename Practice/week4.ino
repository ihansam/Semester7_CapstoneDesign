void setup()
{
  //Serial.begin(9600);
  pinMode(10, INPUT_PULLUP);
  pinMode(13, OUTPUT);
}

void loop()
{
  int sw = digitalRead(10);
  if (sw==LOW){
      digitalWrite(13, HIGH);
  }
  else
  {
      digitalWrite(13, LOW);
  }
  delay(100);
}
