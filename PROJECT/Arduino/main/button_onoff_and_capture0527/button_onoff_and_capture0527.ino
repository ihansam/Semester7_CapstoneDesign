// last update 20.05.31
//lcd 출력부분 고민해보기

// V스위치로 실시간 측정하는 부분 구현해야함 & 알림 onoff기능!!
// 스위치누르기 직전 순간의 자세 정보를 알아야함
// 전역변수 currPos선언 & 자세 센서로 측정시 매 순간 자세 변수 갱신해서 측정 if외부에서 pressed일때 해당하는 자세 LCD에 출력

// V우선순위1.먼저 초음파센서로 거리 측정해서 엉덩이가 멀리 떨어진 모든 경우를 '엉덩이 떨어진자세'로 1차 분류
// 그다음 좌우불균형 판단by FSR
// 마지막으로 이미지센서 ->허리 굽음 
// 총 5개의 자세로 분류함

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

#define TRIG 2
#define ECHO 3

// 5초(SNtime)에 한번 초음파센서 동작위해, cnt = 500일 때 소닉함수 동작시킴
int duration, cm;
// 초음파센서와 인체와의 거리, 기준이 4cm라고 우선 가정
const int distanceSN = 4;

// 5초(FSRtime)에 한번 압력센서에서 input받기 -> cnt = 500일때 측정 
int cnt = 0;
const int sstime = 100; // sstime : 센서측정간격
int sitTime = 0; // 앉은시간측정, 자세가 바르던 나쁘던 앉기만하면됨(FSR에 뭔가 감지되기만 하면)
const int stretchTime = 5000; // 이 값 이상으로 앉은시간(sitTime) 측정되면 스트레칭용 알림전송
unsigned long sitStretch = 0;
const int ledDuration = 500; // led 점멸 시간 간격
int isOn = 0; // LED상태변경 알림 변수 

// 6개의 FSR 센서, 초음파 센서와 먼것부터 1, 2, 3
// 2번이 정자세, 좌우기울어짐 판단
// 1번이 무릎꿇은자세
int L1 = A0;
int R1 = A1;
int L2 = A2; 
int R2 = A3; 
int L3 = A4; 
int R3 = A5; 

const int but_onoff = 5;  // 흰색
int pres_onoff = 0, cnt_onoff = 0; // 실시간 알림기능 onoff용 버튼, 초기상태는 on
const int but_capture = 4; // 버튼 누를 때 lcd에 실시간 자세 알려주는 버튼  // 검정색

const int redPin = 9; // 실시간 알림용 led
const int bluePin = 11; 
const int greenPin = 10;

// 바른자세,엉덩이 멀어진자세, 좌로 치우침, 우로 치우침, 허리만 굽음 순서임
const int posnum = 5;
int posture[posnum] = {0,};
String RTonoff[2] = {" Real-time : ON ", "Real-time : OFF"};
String pos[posnum] = {"  Good Posture! ", " Far from Back! ", "   Bend Left!   ", "   Bend Right!  ", "  Back Curved!  "};
enum POSTURE {Proper=0, Hip_Front, Bend_Left, Bend_Right, Back_Curved};
const int reportTime = 50000; //50초마다 레포트전송
unsigned long prevTime = 0;

int bal;
int check = 0; //센서 측정 시 나쁜자세인지 아닌지 확인하는 값, 0 이면 나쁜자세 기준 중 어느것에도 걸리지 않은것
int currPos = 0; // 현재 자세기록
int pri = 0; //앉아있는 시간이 기준 이상일 때 1로 변화

void setup() {
  Serial.begin(9600);
  
  pinMode(but_onoff, INPUT_PULLUP);
  pinMode(but_capture, INPUT_PULLUP);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Real-time : ON ");
}

void loop() {
  unsigned long currentTime = millis(); // 현재시간 측정

  // valL2, valR2 가 유효한 데이터임
  int valL1 = analogRead(L1);
  int valL2 = analogRead(L2);
  int valL3 = analogRead(L3);
  int valR1 = analogRead(R1);
  int valR2 = analogRead(R2);
  int valR3 = analogRead(R3);

  int val_onoff = digitalRead(but_onoff);
  // cnt_onoff = 0이면, 실시간 알림기능 on
  if(val_onoff == LOW) {
    pres_onoff = 1; 
  }
  else {
    if(pres_onoff == 1) {
      cnt_onoff = !cnt_onoff;
                                        ///////////////////////////////////////////here
      
      if(cnt_onoff == 1) {
        cnt_onoff = 1;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Real-time : OFF");
      }
      else {
        cnt_onoff = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(" Real-time : ON ");       
      }
    }
    pres_onoff = 0;
  }
  Serial.println(cnt_onoff);                              // test


  
  int val_capture = digitalRead(but_capture);
  // 스위치 눌려질때 직전자세 lcd 2번줄에 출력
  if(val_capture == LOW)  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(RTonoff[cnt_onoff]);  // 현재 실시간 알림 모드 on/off 여부 함께 출력
    lcd.setCursor(0, 1);
    lcd.print(pos[currPos]);
  }
  
  // if내부 : 앉아있다는 조건이 성립한다면 진입
  if(valL2 > 10 || valR2 > 10) {
    sitTime += 1;

    // 5초에 한번 나쁜자세인지 아닌지 판단
    if(cnt < sstime) {
      cnt = cnt + 1; 
    }
    else {
      UltraSonic();
      sideBalance(valL2, valR2);

      
      // 초음파센서 측정
      if(cm > distanceSN) { // 기준보다 초음파센서와 거리가 멀 때
        posture[1] += 1;
        Serial.println("올바르지 않은자세: type1, 등받이와 엉덩이 멀어짐");
        check++;
        currPos = 1;
      }
      // 좌로 기울어짐
      else if(bal == 2) {
        posture[2] += 1;
        Serial.println("올바르지 않은자세: type2, 왼쪽으로 기울어짐");
        check++;
        currPos = 2;
      }
      // 우로 기울어짐
      else if(bal == 1) {
        posture[3] += 1;
        Serial.println("올바르지 않은자세: type3, 오른쪽으로 기울어짐");
        check++;
        currPos = 3;
      }

      // 이미지데이터 처리결과반영
      //
      //
      //
      
      
      // 나쁜자세에 안걸리면 바른자세로 간주
      else {
         posture[0] += 1; // 바른자세 카운팅
         Serial.println("바른자세: type0");
         check = 0; // led red위해 누적하던 나쁜자세 카운팅은 초기화됨
         currPos = 0;
      }
      
      // 스트레칭 알림이 나쁜자세알림보다 우선되니까 sitTime < stretchTime 일 때(pri == 0) 자세 알림용 led on
      // 나쁜자세가 연속으로 5이상 측정되는 동안 led red로 불빛 나타남
      // 바른자세로 바꿀 시 다시led green
      
      if(cnt_onoff = 0) {
        if(pri == 0) {
          if(check >= 5){
            analogWrite(redPin, 255);         
            analogWrite(greenPin, 0);
            analogWrite(bluePin, 0); 
            Serial.println("LED : RED");
          }
          else{ 
            analogWrite(redPin, 0);         
            analogWrite(greenPin, 255);
            analogWrite(bluePin, 0);
            Serial.println("LED : GREEN");
          }
        }
      }
      else {
        analogWrite(redPin, 0);         
        analogWrite(greenPin, 0);
        analogWrite(bluePin, 0); 
//        Serial.println("No Real-time notice");
      }
      
      cnt = 0;

    }    
  }
  else {
    sitTime = 0; // 일어나면 앉은시간 초기화
  }

  // sitTime이 기준이상일 때 스트레칭 알림 전송 & LED 점멸
  if(stretchTime <= sitTime) {
    if(pri == 0) {
      pri = 1;
      sitStretch = currentTime; // sittime이 stretchtime넘어가는 순간기록 
      analogWrite(redPin, 255);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, 0);
      isOn = !isOn; // isOn True로 바뀜
      //StretchingAlarm(); //어플로 스트레칭알림
    }
    else{
      if(currentTime > sitStretch + ledDuration) {
        if(isOn){
          analogWrite(redPin, 0); 
          analogWrite(greenPin, 0);
          analogWrite(bluePin, 0);
        }
        else {
          analogWrite(redPin, 255); 
          analogWrite(greenPin, 0);
          analogWrite(bluePin, 0);
        }
        isOn = !isOn;
        sitStretch = currentTime; //시간 갱신
      }
    }
  }

  // reportTime마다 전송
  if(currentTime > prevTime + reportTime) {
    //ShowReport(posture, 6); // 어플로 전송
    prevTime = currentTime;
  }
    
  delay(10);
}

// 좌우불균형 판단함수
// 오른쪽 : 1, 균형 : 0, 왼쪽 : 2
// 왼쪽오른쪽 측정값의 차가 400 이내면 무게가 고르다고 판단
int sideBalance(int L, int R) {
  if(L < R - 400)
  {
    bal = 1;
  }
  else if(L >= R + 400)
  {
    bal = 2;
  }
  else {
    bal = 0;
  }
  return bal;
}


// 초음파 센서로 거리 측정하는 함수
int UltraSonic() 
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH);
  cm = duration / 29 / 2;

  Serial.print("등받이와의 거리 : ");
  Serial.print(cm);
  Serial.println("cm");

  return cm;
}


      /*Serial.println("압력센서 측정값");
      Serial.print("L1:");Serial.print(valL1);
      Serial.print("/ R1:");Serial.println(valR1);
      Serial.print("L2:");Serial.print(valL2);
      Serial.print("/ R2:");Serial.println(valR2);
      Serial.print("L3:");Serial.print(valL3);
      Serial.print("/ R3:");Serial.println(valR3);
      */
