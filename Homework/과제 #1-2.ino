void setup()
{
  Serial.begin(9600);
  
  int i = 9; int j = 1;
  while (j<10){
    Serial.print(i);
    Serial.print(" X ");
    Serial.print(j);
    Serial.print(" = ");
    Serial.println(i*j);
    j++;
  }
}

void loop()
{
  
}
