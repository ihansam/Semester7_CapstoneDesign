void setup()
{
  Serial.begin(9600);
  
  int i, j;
  i = 9;
  for (j=1; j<10; j++){
    Serial.print(i);
    Serial.print(" X ");
    Serial.print(j);
    Serial.print(" = ");
    Serial.println(i*j);
  }
}

void loop()
{
  
}
