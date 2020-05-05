// Kidan Jin, EEE, SKKU
const int REDpin = 10;
const int GREENpin = 9;
const int BLUEpin = 6;
const int SPEAKER = 5;
const int BUTTON = 4;
const int ECHO = 3;
const int TRIG = 2;
int pressed = 1;        // negative active
bool mode = false;      // if false, BGR order, else, RGB order
int RLED = 0;
int GLED = 0;
int BLED = 0;

int measure()
{
  int duration, distance;
  
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH);
  distance = duration/29/2;

  return distance;
}

void setup() {
  Serial.begin(9600);
  pinMode(REDpin, OUTPUT);
  pinMode(GREENpin, OUTPUT);
  pinMode(BLUEpin, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(ECHO, INPUT);
  pinMode(TRIG, OUTPUT);  
}

void loop() {
  // show distance on monitor
  int distance = measure();
  Serial.print(distance); Serial.println("cm");

  // button toggle
  int PRESS = digitalRead(BUTTON);
  if (PRESS == 0) pressed = 0;
  else if (pressed == 0){
    pressed = 1;
    mode = not(mode);
  }

  // RGB value assign
  int VoltValue = map(distance%100, 0, 99, 0, 255);
  if (distance < 100){
    RLED = mode * VoltValue;      // R: mode가 0이면 0, 1이면 voltvalue
    GLED = 0;                     // G: 항상 off
    BLED = not(mode) * VoltValue; // B: mode가 1이면 voltvalue, 1이면 0
  } 
  else if (distance < 200){
    RLED = mode * 255;            // R: mode가 0이면 off, 1이면 on
    GLED = VoltValue;
    BLED = not(mode) * 255;
  }
  else if (distance < 300){       // R: mode가 0이면 voltvalue, 1이면 on
    RLED = not(mode) * VoltValue + mode * 255;
    GLED = 255;
    BLED = mode * VoltValue + not(mode) * 255;
    tone(SPEAKER, 500, 100); delay(100);
  }
  else{
    RLED = 0; GLED = 0; BLED = 0;
  }

  analogWrite(REDpin, RLED);
  analogWrite(GREENpin, GLED);
  analogWrite(BLUEpin, BLED);
    
  delay(100);
}
