//last update 20.06.03
#define inpin 7
#define outpin 3

void setup()
{
  Serial.begin(9600);
  pinMode(outpin, OUTPUT);
  pinMode(inpin, INPUT);
}

void loop()
{
//  analogWrite(outpin, 682);
  int h = digitalRead(inpin);  
  Serial.println(h);
  delay(100);
}
