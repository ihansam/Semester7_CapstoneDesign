// last update 2020.05.22 by Kidan Jin
#include <SoftwareSerial.h>     // 통신(블루투스)을 위한 라이브러리
const int bteTx = 3;            // TXD pin번호
const int bteRx=2;              // RxD pin번호
SoftwareSerial bteSerial(bteTx, bteRx);  // 통신 객체

void ShowReport(int RptArr[], int n)    // 어플에 레포트를 띄우는 instruction을 전달하는 함수
{
  String instruction = "R";
  int sum = 0;
  // 앉아있지 않은 NONE을 제외한 자세 개수 sum을 센다
  for (int i = 1; i < n; ++i) sum += RptArr[i];
  // 각 자세의 비율을 구해 instruction에 추가한다
  if (sum){
    for (int i = 1; i < n; ++i){
      int rate = 100*RptArr[i]/sum;     // instruction format: "R 00 00 00 00 00 00"
      instruction += " ";
      instruction += rate;
    }
  }
  else return;                          // sum = 0이면 instruction을 생성하지 않고 return
/* // for test----------------------
  Serial.println(instruction);
  // ------------------------------
*/
  bteSerial.print(instruction);
}

// 각 센서 값과 라즈베리가 보낸 값을 통해 최종적으로 자세를 판단하는 함수
int EvaluatePosture(/*int 초음파, int 압력[], int 라즈베리*/){
  return random(0, 7);  // ^^;
}

void setup()
{
  Serial.begin(9600);
  bteSerial.begin(9600);
}

enum posture {NONE, PROPER, TURTLE_NECK, HIP_FRONT, BEND_LEFT, BEND_RIGHT, KNEEL};
const int posnum = 7;
const int sensingPeriod = 100;

void loop()
{
  int report[posnum] = {0,};
  int currPos = -1;
  while (currPos){                  // 현재 자세가 NONE이 아닐 때까지 레포트 생성
    currPos = EvaluatePosture();    // 현재 자세 평가 결과를 받아옴
    ++report[currPos];              // report의 currPos번째 값을 ++
    delay(sensingPeriod);
  }
/*// for test 나중에 지울 것 ----------------------------------
  Serial.println("=======REPORT=======");
  for (int i=0; i<posnum; ++i){
    Serial.print(report[i]); Serial.print(", ");
  }
  Serial.println();                      
// --------------------------------------------------------
*/
  ShowReport(report, posnum);               // 함수 호출
  delay(10000);
}
