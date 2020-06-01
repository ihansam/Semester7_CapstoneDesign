//last update 20.06.01
//lcd 출력부분 고민해보기

// V스위치로 실시간 측정하는 부분 구현해야함 & 알림 onoff기능!!
// 스위치누르기 직전 순간의 자세 정보를 알아야함
// 전역변수 currPos선언 & 자세 센서로 측정시 매 순간 자세 변수 갱신해서 측정 if외부에서 pressed일때 해당하는 자세 LCD에 출력

// V우선순위1.먼저 초음파센서로 거리 측정해서 엉덩이가 멀리 떨어진 모든 경우를 '엉덩이 떨어진자세'로 1차 분류
// 그다음 좌우불균형 판단by FSR
// 마지막으로 이미지센서 ->허리 굽음 
// 총 5개의 자세로 분류함

// 라이브러리 및 객체 선언 ----------------------------------------------
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

// 핀 번호 할당 ---------------------------------------------------------
//const int L1 = A0;              // 앞
//const int R1 = A1;
#define L2 A2                     // 압력 센서 핀
#define R2 A3  
//const int L3 = A4;              // 뒤
//const int R3 = A5; 
#define TRIG 2                    // 초음파 센서 핀
#define ECHO 3
#define redPin 9                  // 실시간 알림용 led 핀
#define greenPin 10
#define bluePin 11   
#define btn_onoff 5               // 실시간 LED 알림 onoff 버튼 (흰색)
#define btn_capture 4             // 실시간 자세 LCD 확인 버튼 (검정색)

// Parameters ------------------------------------------------------------
// 자세의 개수 및 이름
const int posnum = 5;
enum POSTURES {PROPER=0, HIP_FRONT, BEND_LEFT, BEND_RIGHT, BACK_CURVED};
// LCD 첫째 줄 문구 (실시간 LED 알림 on/off 상태)
String RTonoff[2] = {"Real-time : OFF", " Real-time : ON "};
// LCD 둘째 줄 문구 (현재 자세)
String pos[posnum] = {"  Good Posture! ", " Far from Back! ", "   Bend Left!   ", "   Bend Right!  ", "  Back Curved!  "};
const int distanceSN = 4;         // 엉덩이가 떨어져 앉은 것으로 판단하는 초음파 센서 거리 기준
const int notBalanceDiff = 400;   // 좌우 불균형으로 판단하는 압력 센서 차이 값 기준
const int sstime = 10;            // 자세 판별 간격: 1초
const int maxDATA = 10;           // 데이터가 이만큼 모이면 데이터 분석 및 어플로 REPORT 전송
const int stretchTime = 200;      // 스트레칭 알림 주기 (20초 이상 오래 앉아있을 시 알림)
const int ledDuration = 500;      // led 점멸 시간 간격
const int loopPeriod = 100;       // main loop 간격

// Grobal Variables --------------------------------------------------------
bool btn_onoff_pushed = false;    // LED 알림 onoff 버튼이 이전 clock에 눌렸었는지
bool btn_capture_pushed = false;  // LCD 표시 버튼이 이전 clock에 눌렸었는지
int isRealtimeON = 1;             // 실시간 LED 알림 모드 ON/OFF 상태 (초기 1)
int sscnt = 0;                    // 매 loop마다 count하여 sstime이 되면 자세 판별
int currPos = PROPER;             // 현재 자세
int posDATA[posnum] = {0,};       // 판단 자세 결과 누적 배열 (각 인덱스는 enum POSTURES의 index와 일치)
int numDATA = 0;                  // posDATA에 모인 데이터 개수
int sitTime = 0;                  // 앉은 누적 시간 측정 (자세가 바르던 나쁘던 앉기만하면됨 = FSR에 뭔가 감지되기만 하면)
unsigned long prevLEDtime;        // led 점멸을 위한 시간 측정
bool isLEDon = false;             // 점멸하는 led의 onoff 상태

// FUCNTIONS --------------------------------------------------------------
bool checkSIT(int l, int r){          // NEEDMODIFY
  if (l>=450 && l<550 && r>=450 && r<550)
    return false;
  return true;
}

int UltraSonic() 
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  int duration = pulseIn(ECHO, HIGH);
  int cm = duration / 29 / 2;

  Serial.print("등받이와의 거리 : ");   // DEBUG
  Serial.print(cm);
  Serial.println("cm");

  return cm;
}

// 좌우 불균형 판단함수 (오른쪽 : 1, 균형 : 0, 왼쪽 : 2)
// 왼쪽 오른쪽 압력센서 측정값의 차가 400 이내면 무게가 고르다고 판단
int sideBalance(int L, int R) {
  if(L < R - notBalanceDiff){
    return 1;
  }
  else if(L >= R + notBalanceDiff) {
    return 2;
  }
  else {
    return 0;
  }
}

bool checkBackBend(){
  // 라즈베리파이로부터 현재 이미지 판독 결과를 받아오는 부분
  // if (^^) return true;
  return false;
}

void RGBWrite(int r, int g, int b){
  analogWrite(redPin, r);
  analogWrite(greenPin, g);
  analogWrite(bluePin, b);
}

// MAIN =========================================================================
void setup()
{
  Serial.begin(9600);
  
  pinMode(btn_onoff, INPUT_PULLUP);
  pinMode(btn_capture, INPUT_PULLUP);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(RTonoff[isRealtimeON]);

  RGBWrite(0,0,0);
}

void loop()
{
  // 앉았는지 확인하는 부분 -------------------------------------
  int valL = analogRead(L2);
  int valR = analogRead(R2);
  bool isSIT = checkSIT(valL, valR);

  if(isSIT) ++sitTime;
  else {
    sitTime = 0;                        // 앉은 시간과 현재 자세 초기화
    currPos = PROPER;
  }

  Serial.print(valL);                   // DEBUG
  Serial.print(" ");
  Serial.print(valR);
  Serial.print(" ");
  Serial.println(isSIT);
  
  // 각 버튼이 눌렸는지를 감지하는 부분 ---------------------------
  int btn_onoff_pushing = digitalRead(btn_onoff);
  int btn_capture_pushing = digitalRead(btn_capture);

  if(btn_onoff_pushing == LOW){
    btn_onoff_pushed = true;    
  }
  else{
    if(btn_onoff_pushed == true){
      isRealtimeON = (isRealtimeON + 1) % 2;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(RTonoff[isRealtimeON]);
      btn_onoff_pushed = false;
    }      
  }
  
  if(btn_capture_pushing == LOW){
    btn_capture_pushed = true;    
  }
  else{
    if(btn_capture_pushed == true){
      if(isSIT){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(RTonoff[isRealtimeON]);
        lcd.setCursor(0, 1);
        lcd.print(pos[currPos]);
      }
      btn_capture_pushed = false;
    }      
  }

  // 자세 판단 부분 --------------------------------------------------------------
  if(isSIT){
    if(sscnt < sstime) ++sscnt;
    else{                                         // 자세 판단 시간이 되었을 때
      sscnt = 0;
      if(UltraSonic() > distanceSN)               // 초음파 센서 체크
        currPos = HIP_FRONT;                      // 1. 엉덩이를 붙이지 않은 자세
      else{ 
        int checkBalance = sideBalance(valL, valR);   // 압력 센서 체크
        if (checkBalance == 2){
          currPos = BEND_LEFT;                    // 2. 왼쪽으로 기운 자세
        }
        else if (checkBalance == 1){
          currPos = BEND_RIGHT;                   // 3. 오른쪽으로 기운 자세
        }
        else{
          if (checkBackBend())                    /// 이미지 센서 체크
            currPos = BACK_CURVED;                // 4. 허리가 굽은 자세
          else currPos = PROPER;                  // 0. 바른 자세
        }
      }
      posDATA[currPos] += 1;                      // 판단 자세 결과를 posDATA에 누적
      ++numDATA;
      Serial.println("==================현재 자세==================");      // DEBUG
      Serial.println(pos[currPos]);
      Serial.println("=============================================");
    }
  }    

  // RGB LED 출력 부분 -----------------------------------------------------------
  unsigned long currentTime = millis();
  if (sitTime == 0) RGBWrite(0,0,0);              // 앉아있지 않으면 LED off
  else if (sitTime == stretchTime){               // 스트레칭 알림이 실시간 알림보다 우선
    //StretchingAlarm();
  }
  else if (sitTime > stretchTime){
    if(currentTime > prevLEDtime + ledDuration){
      prevLEDtime = currentTime;
      isLEDon = !isLEDon;
      RGBWrite(255*isLEDon, 255*isLEDon, 0);      // isLEDon이 true면 ON, false면 OFF
    }
  }
  else{                                           // 스트레칭 시간이 아니면 실시간 알림
    if(isRealtimeON && isSIT){                    // 실시간 알림이 ON이고, 앉아있을 때만 동작
      if(currPos) RGBWrite(255, 0, 0);            // currPos가 0이 아니면 나쁜 자세이므로, 빨간색
      else RGBWrite(0, 255, 0);                   // 0이면 바른 자세이므로, 초록색
    }
    else RGBWrite(0,0,0);
  }
  
  // 자세 데이터가 일정 개수 모이면 분석 후 REPORT를 생성하고 어플에 보내는 부분
  if(numDATA >= maxDATA){
    // ShowReport(posDATA, posnum);
    numDATA = 0;
    for (int i=0; i<posnum; ++i) posDATA[i]=0;    // posDATA 초기화
  }

  delay(loopPeriod);
}



/*

// 5초(SNtime)에 한번 초음파센서 동작위해, cnt = 500일 때 소닉함수 동작시킴
int duration, cm;
// 초음파센서와 인체와의 거리, 기준이 4cm라고 우선 가정


const int stretchTime = 5000; // 이 값 이상으로 앉은시간(sitTime) 측정되면 스트레칭용 알림전송
unsigned long sitStretch = 0;
const int ledDuration = 500; // led 점멸 시간 간격
int isOn = 0; // LED상태변경 알림 변수 




int pres_onoff = 0, cnt_onoff = 0; // 실시간 알림기능 onoff용 버튼, 초기상태는 on



// 바른자세,엉덩이 멀어진자세, 좌로 치우침, 우로 치우침, 허리만 굽음 순서임
const int posnum = 5;
int posture[posnum] = {0,};

const int reportTime = 50000; //50초마다 레포트전송
unsigned long prevTime = 0;

int bal;
int check = 0; //센서 측정 시 나쁜자세인지 아닌지 확인하는 값, 0 이면 나쁜자세 기준 중 어느것에도 걸리지 않은것

int pri = 0; //앉아있는 시간이 기준 이상일 때 1로 변화



void loop() {
  unsigned long currentTime = millis(); // 현재시간 측정

  // valL2, valR2 가 유효한 데이터임
  int valL1 = analogRead(L1);
  int valL2 = analogRead(L2);
  int valL3 = analogRead(L3);
  int valR1 = analogRead(R1);
  int valR2 = analogRead(R2);
  int valR3 = analogRead(R3);

  


  int val_onoff = digitalRead(btn_onoff);
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


  
  int val_capture = digitalRead(btn_capture);
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



// 초음파 센서로 거리 측정하는 함수

*/
      /*Serial.println("압력센서 측정값");
      Serial.print("L1:");Serial.print(valL1);
      Serial.print("/ R1:");Serial.println(valR1);
      Serial.print("L2:");Serial.print(valL2);
      Serial.print("/ R2:");Serial.println(valR2);
      Serial.print("L3:");Serial.print(valL3);
      Serial.print("/ R3:");Serial.println(valR3);
      */
