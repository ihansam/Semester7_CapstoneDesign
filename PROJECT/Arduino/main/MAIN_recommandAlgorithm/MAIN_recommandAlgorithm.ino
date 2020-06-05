// last update 20.06.05
// 핀 번호 할당 ---------------------------------------------------------
#define L2 A3                     // 압력 센서 핀 left, right, front
#define R2 A2
#define F2 A1
#define TRIG 2                    // 초음파 센서 핀
#define ECHO 3
#define redPin 9                  // 실시간 알림용 led 핀
#define greenPin 10
#define bluePin 11   
#define btn_onoff 5               // 실시간 LED 알림 onoff 버튼 (흰색)
#define btn_capture 4             // 실시간 자세 LCD 확인 버튼 (검정색)
#define bteTx 14                  // TXD pin번호
#define bteRx 15                  // RxD pin번호
#define MSB A8                    // 라즈베리 통신 pin번호
#define LSB A9

// 라이브러리 및 객체 선언 ----------------------------------------------
#include <LiquidCrystal_I2C.h>    // LCD
LiquidCrystal_I2C lcd(0x27,16,2);
#include <SoftwareSerial.h>       // 블루투스모듈
SoftwareSerial bteSerial(bteTx, bteRx);

// Parameters ------------------------------------------------------------
// 자세의 종류 및 개수
enum POSTURES {PROPER=0, HIP_FRONT, BEND_LEFT, BEND_RIGHT, BACK_CURVED};
const int posnum = 5;
// LCD 윗줄 출력 문구: LED 실시간 알림 모드 ON/OFF             
String RTonoff[2] = {"Real-time : OFF", " Real-time : ON "};  // LCD 첫째 줄 문구 (실시간 LED 알림 on/off 상태)
// LCD 아래줄 문구: 현재 자세
String pos[posnum] = {"  Good Posture! ", " Far from Back! ", "   Bend Left!   ", "   Bend Right!  ", "  Back Curved!  "};
const float distanceSN = 4.00;    // 엉덩이가 떨어져 앉은 것으로 판단하는 초음파 센서 거리 기준
const int notBalanceDiff = 200;   // 좌우 불균형으로 판단하는 압력 센서 차이 값 기준
const int minWeight = 100;        // 앉았다고 판단하는 압력 센서 값
const int sstime = 10;            // 자세 판별 간격: 1초
const int maxDATA = 100;          // 데이터가 이만큼 모이면 데이터 분석 및 어플로 REPORT 전송
const int stretchTime = 70;       // 스트레칭 알림 주기 (7초 이상 오래 앉아있을 시 알림)
const int ledDuration = 500;      // led 점멸 시간 간격: 500ms
const int loopPeriod = 100;       // main loop 간격: 100ms
const int BADnum = 3;             // 3초 이상 나쁜 자세 유지시 LED 실시간 알림
enum BERRYSIGNAL {NOT_PREPARED=0, UNCERTAIN, CURVED, STRAIGHT};
const int uncertainMax = 3;       // 3초 이상 자세 판독 불가시 카메라 위치 재설정 알림

// Global Variables --------------------------------------------------------
bool btn_onoff_pushed = false;    // LED onoff 버튼이 직전 loop에서 눌렸었는지
bool btn_capture_pushed = false;  // LCD 표시 버튼이 직전 loop에서 눌렸었는지
int isRealtimeON = 1;             // 실시간 LED 알림 Mode의 ON/OFF 상태 (초기 on)
int sscnt = 0;                    // 자세 판별 간격 측정 변수 (sstime이 되면 판별)
int currPos = PROPER;             // 판별된 현재 자세를 담는 변수
int posDATA[posnum] = {0,};       // 판단 자세 결과 누적 (각 index는 enum POSTURES의 index와 일치)
int numDATA = 0;                  // posDATA에 모인 데이터 개수
int sitTime = 0;                  // 앉은 누적 시간 측정 (자세가 바르던 나쁘던 앉기만 하면 = FSR에 뭔가 감지되기만 하면)
unsigned long prevLEDtime = 0;    // led 점멸을 위한 시간 측정
unsigned long currentTime = 0;    // 시스템 동작 시간
bool isLEDon = false;             // 점멸하는 led의 onoff 상태
int badCnt = 0;                   // 연속 나쁜 자세 개수 카운트
int currBerrySig = NOT_PREPARED;  // 현재 라즈베리가 보낸 신호

// FUCNTIONS --------------------------------------------------------------
// 압력 센서 값을 읽어, 앉아있는지 여부를 반환하는 함수, 앉아있다면 TRUE 반환
bool checkSIT(int l, int r, int f)
{
  if (l >= minWeight || r >= minWeight || f >= minWeight )
    return true;
  else
    return false;
}

// 등받이와 엉덩이 사이 거리를 측정해 반환하는 함수
float UltraSonic() 
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  int duration = pulseIn(ECHO, HIGH);
  float cm = (float)duration / 29 / 2;

  Serial.print("등받이와의 거리 : ");   // DEBUG
  Serial.print(cm);
  Serial.println("cm");

  return cm;
}

// 압력 센서 값을 통해 좌우 불균형을 판단하는 함수 (오른쪽 : 1, 균형 : 0, 왼쪽 : 2)
int sideBalance(int L, int R) 
{
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

// 라즈베리파이가 보내는 신호를 읽는 함수
int CheckRaspberryPin()
{
  int m1 = analogRead(MSB);
  int m2 = analogRead(LSB);
  
  if (m1 > 500 && m2 > 500) 
    return STRAIGHT;
  else if (m1 > 500 && m2 <= 500) 
    return CURVED;
  else if (m1 <= 500 && m2 >= 500) 
    return UNCERTAIN;
  else 
    return NOT_PREPARED;
}

// 어플로 카메라 위치를 재설정하라는 경고를 보내는 함수
void CameraErrorAlarm()
{
  static int erroridx = 0;
  String instruction = "C";
  instruction += erroridx++;
  bteSerial.print(instruction);
}

// 어플로 카메라 위치가 적절함을 알리는 함수
void CameraSettingDone()
{
  static int doneidx = 0;
  String instruction = "D";
  instruction += doneidx++;
  bteSerial.print(instruction);
}

void CameraCheck(int sig)
{
  static int UncertainCnt = 0;
  if (sig == UNCERTAIN) {                       // 판독 불가 signal을 지속적으로 받으면
    if(UncertainCnt == uncertainMax) CameraErrorAlarm();  // 카메라 위치 재설정 경고 보냄
    ++UncertainCnt;
  }
  else if (UncertainCnt > 0) {                  // 카메라 위치를 잘 잡아 판독이 가능해지면
    UncertainCnt = 0;                           // 카메라 위치 설정이 잘 되었다는 알림 보냄
    CameraSettingDone();
  }  
}

// RGB LED에 r,g,b값에 해당하는 색깔을 출력하는 함수
void RGBWrite(int r, int g, int b)
{
  analogWrite(redPin, r);
  analogWrite(greenPin, g);
  analogWrite(bluePin, b);
}

// 어플에 레포트를 띄우기 위해 instruction을 생성하고 전달하는 함수
void ShowReport(int RptArr[], int n)
{
  String instruction = "R";
  
  int sum = 0;                          // 총 데이터 개수를 센다
  for (int i = 0; i < n; ++i)
    sum += RptArr[i];
    
  if (sum){                             // 각 자세의 비율을 구해 instruction에 추가한다
    for (int i = 0; i < n; ++i){
      int rate = 100*RptArr[i]/sum;     // instruction format: "R 00 00 00 00 00"
      instruction += " ";
      instruction += rate;
    }
  }
  else return;                          // sum = 0이면 instruction을 생성하지 않고 return
  bteSerial.print(instruction);
}

// 추천 스트레칭을 선정하여 어플에 전달하는 함수
void StretchingAlarm(int RptArr[], int n)
{                                       // 스트레칭 추천 알고리즘
  int recommendNUM = 0;                 // 스트레칭 번호는 1~5로 각 링크 주소는 어플에 저장됨
  int sum = 0;                    
  for (int i = 0; i < n; ++i)
    sum += RptArr[i];                   // 현재까지 모인 자세 데이터 개수를 센다
    
  if (sum < 5)                          // 정보가 충분하지 않으면 랜덤 스트레칭 추천
    recommendNUM = random(1,6);        
  else if (RptArr[0] < sum/5)           // 바른 자세 비율이 20% 미만이면
    recommendNUM = 5;                   // 강도 높은 스트레칭 추천
  else if (RptArr[4] >= sum/2)          // 허리 굽은 자세 비율이 50% 이상이면
    recommendNUM = 4;                   // 허리 피는 스트레칭 추천
  else                                  // 그 외에는 의자에서 할 수 있는
    recommendNUM = random(1,4);         // 간단한 스트레칭을 추천
    
  String instruction = "S";
  instruction += recommendNUM;
  bteSerial.print(instruction);         // S1 ~ S5라는 instruction을 어플로 전달
}

// MAIN =========================================================================
void setup()
{
  Serial.begin(9600);
  bteSerial.begin(9600);
  Serial.println("[Program Start]");
  
  pinMode(btn_onoff, INPUT_PULLUP);     // pin mapping
  pinMode(btn_capture, INPUT_PULLUP);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  lcd.init();                           // lcd setup
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(RTonoff[isRealtimeON]);
  Serial.println("[LCD Setup Clear]");

  RGBWrite(0,0,0);                      // led setup
  Serial.println("[LED Setup Clear]");

  currBerrySig = CheckRaspberryPin();   // 라즈베리 딥러닝 모델 준비 완료 여부 확인
  while(currBerrySig == NOT_PREPARED) {
    Serial.println("[waiting for raspberry...]");
    delay(1000);
    currBerrySig = CheckRaspberryPin();
  }
  Serial.println("[Raspberry Setup Clear]");
  
  while(currBerrySig == UNCERTAIN){     // 카메라 정상 동작 여부 확인
    CameraCheck(currBerrySig);
    Serial.println("[please set camera properly!]");
    delay(1000);
    currBerrySig = CheckRaspberryPin();
  }
  CameraCheck(currBerrySig);
  Serial.println("[Camera Works Properly]");
  Serial.println("[System Setup Done]");
}

void loop()
{
  // 앉았는지 확인하는 부분 -------------------------------------
  int valL = analogRead(L2);            // read 압력센서
  int valR = analogRead(R2);
  int valF = analogRead(F2);
  bool isSIT = checkSIT(valL, valR, valF);  // 앉았는지 판단 => isSIT

  if(isSIT)
    ++sitTime;                          // 앉았으면 앉은 시간 ++
  else {                                // 앉지 않았으면 앉은 시간, 현재 자세,
    sitTime = 0;                        // 누적 나쁜 자세 개수 초기화
    currPos = PROPER;
    badCnt = 0;
  }
  
  // 각 버튼이 눌렸는지를 감지하는 부분 ---------------------------
  int btn_onoff_pushing = digitalRead(btn_onoff);
  int btn_capture_pushing = digitalRead(btn_capture);

  if(btn_onoff_pushing == LOW){               // LED 실시간 onoff 모드 전환 버튼
    btn_onoff_pushed = true;    
  }
  else{
    if(btn_onoff_pushed == true){             // 버튼이 눌리면
      isRealtimeON = (isRealtimeON + 1) % 2;  // Mode를 전환하고
      lcd.clear();                            // LCD 첫 줄에 띄운다
      lcd.setCursor(0, 0);
      lcd.print(RTonoff[isRealtimeON]);       // isRealtimeON이 0이면 off, 1은 on
      btn_onoff_pushed = false;
    }      
  }
  
  if(btn_capture_pushing == LOW){             // LCD 현재 자세 표시 버튼
    btn_capture_pushed = true;    
  }
  else{
    if(btn_capture_pushed == true){           // 버튼이 눌리면
      if(isSIT){                              // 그리고 앉아있다면
        lcd.clear();                          // LCD 첫 줄에 현재 LED onoff 모드를
        lcd.setCursor(0, 0);
        lcd.print(RTonoff[isRealtimeON]);
        lcd.setCursor(0, 1);                  // 둘째 줄에 현재 자세를 출력
        lcd.print(pos[currPos]);              // pos배열의 currPos번째 문구 = 현재 자세 문구
      }
      btn_capture_pushed = false;
    }      
  }

  // 자세 판단 부분 --------------------------------------------------------------
  if(isSIT){
    if(sscnt < sstime){
      ++sscnt;                                  // 자세 판단 간격 count
    }
    else{                                       // 자세 판단 시간이 되었을 때
      sscnt = 0;
      currBerrySig = CheckRaspberryPin();           // 라즈베리가 보낸 신호 감지
      CameraCheck(currBerrySig);                    // 카메라 상태를 점검

      if(UltraSonic() > distanceSN)                 // 1st: 초음파 센서 거리 체크
        currPos = HIP_FRONT;                        // => [1] 엉덩이를 붙이지 않은 자세
      else{ 
        int checkBalance = sideBalance(valL, valR); // 2nd: 압력 센서 좌우 기울어짐 체크
        if (checkBalance == 2){
          currPos = BEND_LEFT;                      // => [2] 왼쪽으로 기운 자세
        }
        else if (checkBalance == 1){
          currPos = BEND_RIGHT;                     // => [3] 오른쪽으로 기운 자세
        }
        else{
          if (currBerrySig == CURVED)               // 3rd: 이미지 센서 허리 굽음 체크
            currPos = BACK_CURVED;                  // => [4] 허리가 굽은 자세
          else if (currBerrySig == STRAIGHT)
            currPos = PROPER;                       // => [0]. 바른 자세
        //else                                      // 이외의 상황에서는 실시간 자세를 업데이트하지 않음
        }
      }

      posDATA[currPos] += 1;                        // 판단 자세 결과를 posDATA에 누적
      ++numDATA;
      
      if(currPos)
        ++badCnt;                                   // 판단 결과가 나쁜 자세면 badcnt++
      else
        badCnt = 0;                                 // 판단 결과가 바른 자세면 badcnt 리셋
        
      // DEBUG ////////////////////////////////////////////////////////////////////////
      Serial.print("LEFT: "); 
      Serial.print(valL);
      Serial.print(" / RIGHT: ");
      Serial.print(valR);
      Serial.print(" / FRONT: ");
      Serial.println(valF);
      Serial.print("sitTime: ");
      Serial.println(sitTime);
      Serial.print("dataNUM: ");
      Serial.println(numDATA);
      Serial.print("badCnt: ");
      Serial.println(badCnt);
      Serial.println("==================현재 자세==================");
      Serial.println(pos[currPos]);
      Serial.println("=============================================");
      Serial.print("라즈베리에서 받은 신호: ");
      if (currBerrySig == NOT_PREPARED)
        Serial.println("딥러닝 모델 준비 미완료");
      else if (currBerrySig == UNCERTAIN)
        Serial.println("자세 판독 불가");
      else if (currBerrySig == CURVED)
        Serial.println("허리가 굽어있음");
      else
        Serial.println("허리가 펴져있음");
      ///////////////////////////////////////////////////////////////////////////////////
    }
  }    

  // RGB LED 출력 부분 -----------------------------------------------------------
  currentTime = millis();                         // 현재 시간 측정
  if (!isSIT)
    RGBWrite(0,0,0);                              // 앉아있지 않으면 LED off
  else if (sitTime == stretchTime){               // 스트레칭 시간까지 오래 앉아있을 때
    StretchingAlarm(posDATA, posnum);             // 어플로 스트레칭 알림 전송
  }
  else if (sitTime > stretchTime){                // 스트레칭 알림이 갔음에도 계속 앉아있다면
    if(currentTime > prevLEDtime + ledDuration){  // LED 점멸
      prevLEDtime = currentTime;
      isLEDon = !isLEDon;
      RGBWrite(255*isLEDon, 0, 0);
    }
  }
  else if (isRealtimeON){                         // 아직 스트레칭 시간이 아니고 실시간 알림이 on이면
    if(badCnt >= BADnum)
      RGBWrite(255, 0, 0);                        // 나쁜 자세가 일정 시간 지속되면 빨간 색
    else
      RGBWrite(0, 255, 0);                        // 그 외 초록색
  }
  else
    RGBWrite(0,0,0);                              // 앉아있지만, 실시간 알림이 off면 LED off
  
  // 자세 데이터를 모아 분석 REPORT를 생성하고 어플에 보내는 부분 -------------------
  if(numDATA >= maxDATA){
    ShowReport(posDATA, posnum);
    numDATA = 0;
    for (int i=0; i<posnum; ++i)
      posDATA[i]=0;                               // posDATA 초기화
  }

  delay(loopPeriod);
}
