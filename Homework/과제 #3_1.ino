void setup()
{
  Serial.begin(9600);           
  pinMode(10, INPUT_PULLUP);    // 버튼 input
  pinMode(13, OUTPUT);          // LED output
}

int Lpressed = 1;               // 버튼 눌렸었는지 여부, negative active
bool Lflash = false;            // LED 점멸 여부
bool isLeftON = false;          // LED 점등 여부
int Lcnt = 0;                   // 켜진 시간 0.1초씩 count

void loop()
{
  int LPRESS = digitalRead(10);     // 현재 버튼 눌림 여부 확인 
                                    // 눌린 순간 변수 업데이트
  if(LPRESS == LOW){if(Lpressed == HIGH) Lpressed = 0;}
  else{                             
    if(Lpressed == LOW){            
      Lpressed = 1;                 // 떼진 순간 변수 업데이트
      Lflash = not(Lflash);         // LED 점멸 여부 전환
    }
  }

  if(Lflash == true){               // LED가 점멸 모드일 때
    if(++Lcnt%5 == 0){              // 0.5초마다 on/off 전환
      isLeftON = not(isLeftON);
      Serial.println();             // for test with monitor
      if(isLeftON==true) tone(3, 700, 100);
      else tone(3, 500, 100);       // on/off에 따른 틱톡 소리
    }
    Serial.print(isLeftON);         // for test with monitor
    Serial.print(" ");
  }
  else{                             // LED가 점멸 모드가 아닐 때
    isLeftON = false;               // LED off
    Lcnt = 0;                       // count변수 초기화
  }
  digitalWrite(13, isLeftON);       // LED 점등 여부 그대로 LED 제어
  delay(100);
}
