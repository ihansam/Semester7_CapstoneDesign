// last update 20.05.24 by Kidan Jin
const int L1 = A0;
const int R1 = A1;
const int L2 = A2; 
const int R2 = A3; 
const int L3 = A4; 
const int R3 = A5; 
#define TRIG 2
#define ECHO 3

void setup()
{
  Serial.begin(9600);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
}

unsigned long currT = 0;
unsigned long trigT = 0;
int cnt = 0;
const int cntmax = 10;

void loop()
{
  if(++cnt <= cntmax){
      int valL1 = 1023 - analogRead(L1);
      int valL2 = 1023 - analogRead(L2);
      int valL3 = 1023 - analogRead(L3);
      int valR1 = 1023 - analogRead(R1);
      int valR2 = 1023 - analogRead(R2);
      int valR3 = 1023 - analogRead(R3);
      int distance = UltraSonic();
      Serial.print("[ back ]\t"); Serial.print(valL3); Serial.print("\t"); Serial.println(valR3); 
      Serial.print("[middle]\t"); Serial.print(valL2); Serial.print("\t"); Serial.println(valR2); 
      Serial.print("[front ]\t"); Serial.print(valL1); Serial.print("\t"); Serial.println(valR1); 
      Serial.print("[distance]\t"); Serial.print(distance); Serial.println(" cm");
      Serial.println("======================================");    
      delay(1000);
    
  }
  else {cnt = 0; delay(10000);}
}

int UltraSonic() 
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  return pulseIn(ECHO, HIGH)/58;
}
