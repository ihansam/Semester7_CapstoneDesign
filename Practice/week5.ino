// 스위치를 누를 때마다 (즉, 눌렀다 떼면) count를 하고 부저를 울리는 프로그램
void setup()
{
  Serial.begin(9600);
  pinMode(10, INPUT_PULLUP);
}

int pressed = 1;
int cnt = 0;

void loop()
{
  int PRESS = digitalRead(10);
  
  if(PRESS==LOW){
    if(pressed == HIGH){
      pressed = 0;
    }
  }
  else{
    if(pressed == LOW) {
      pressed = 1;
      cnt++;
      Serial.println(cnt);
      tone(3, 500, 100);
    }
  }
  delay(100);
}

// cnt 토글 LED 제어
void setup()
{
  Serial.begin(9600);
  pinMode(10, INPUT_PULLUP);
  pinMode(13, OUTPUT);
}

int pressed = 1;
int cnt = 0;

void loop()
{
  int PRESS = digitalRead(10);
  
  if(PRESS==LOW){
    if(pressed == HIGH){
      pressed = 0;
    }
  }
  else{
    if(pressed == LOW) {
      pressed = 1;
      cnt = (++cnt)%2;
      Serial.println(cnt);
      digitalWrite(13, cnt);
      //tone(3, 500, 100);
    }
  }
  delay(100);
}

// 버튼을 n번 누르면 5n초 켜지는 LED
void setup()
{
  Serial.begin(9600);
  pinMode(10, INPUT_PULLUP);
  pinMode(13, OUTPUT);
}

int pressed = 1;
int cnt = 0;

void loop()
{
  int PRESS = digitalRead(10);
  
  if(PRESS==LOW){if(pressed == HIGH) pressed = 0;}
  else{
    if(pressed == LOW) {
      pressed = 1;
      cnt += 50;
    }
  }
  if(cnt>0){
      Serial.println(cnt);
      --cnt;
      digitalWrite(13, HIGH);
  }
  else digitalWrite(13, LOW);
  delay(100);
}
