void setup()
{
  Serial.begin(9600);           
  pinMode(10, INPUT_PULLUP);    // 버튼 input
  pinMode(9, INPUT_PULLUP);    
  pinMode(13, OUTPUT);          // LED output
  pinMode(12, OUTPUT);          
}

int Lpressed = 1;               // 버튼 눌렸었는지 여부, negative active
bool Lflash = false;            // LED 점멸 여부
bool isLeftON = false;          // LED 점등 여부
int Lcnt = 0;                   // 켜진 시간 0.1초씩 count
int Rpressed = 1;               
bool Rflash = false;            
bool isRightON = false;         
int Rcnt = 0;                   

void loop()
{
  int LPRESS = digitalRead(10);     // 현재 버튼 눌림 여부 확인 
  int RPRESS = digitalRead(9);     
                                    // 눌린 순간 변수 업데이트
  if(LPRESS == LOW){if(Lpressed == HIGH) Lpressed = 0;}
  else{                             
    if(Lpressed == LOW){            
      Lpressed = 1;                 // 떼진 순간 변수 업데이트
      Lflash = not(Lflash);         // LED 점멸 여부 전환
    }
  }
  if(RPRESS == LOW){if(Rpressed == HIGH) Rpressed = 0;}
  else{                             
    if(Rpressed == LOW){            
      Rpressed = 1;                 // 떼진 순간 변수 업데이트
      Rflash = not(Rflash);         // LED 점멸 여부 전환
    }
  }

  if(Lflash&&Rflash == 1){
    if(Lcnt != 0 && Rcnt == 0) {Lcnt = 0; isLeftON = false;}
    else if (Lcnt == 0 && Rcnt != 0) {Rcnt = 0; isRightON = false;}
  }
  if(Lflash == true){               // LED가 점멸 모드일 때
    if(++Lcnt%5 == 0){              // 0.5초마다 on/off 전환
      isLeftON = not(isLeftON);
      if(Rflash == false){
        Serial.println();             // for test with monitor
        if(isLeftON==true) tone(3, 700, 100);
        else tone(3, 500, 100);     // on/off에 따른 틱톡 소리
      }
    }
    Serial.print(isLeftON);         // for test with monitor
    Serial.print(" ");
  }
  else{                             // LED가 점멸 모드가 아닐 때
    isLeftON = false;               // LED off
    Lcnt = 0;                       // count변수 초기화
  }  
  if(Rflash == true){               // LED가 점멸 모드일 때
    if(++Rcnt%5 == 0){              // 0.5초마다 on/off 전환
      isRightON = not(isRightON);
      Serial.println();             // for test with monitor
      if(isRightON==true) tone(3, 700, 100);
      else tone(3, 500, 100);       // on/off에 따른 틱톡 소리
    }
    Serial.print(isRightON);        // for test with monitor
    Serial.print(" ");
  }
  else{                             // LED가 점멸 모드가 아닐 때
    isRightON = false;              // LED off
    Rcnt = 0;                       // count변수 초기화
  }
  digitalWrite(13, isLeftON);       // LED 점등 여부 그대로 LED 제어
  digitalWrite(12, isRightON);      // LED 점등 여부 그대로 LED 제어
  delay(100);
}
