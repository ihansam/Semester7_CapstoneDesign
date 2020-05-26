// PSEUDO CODE for Data Collecting

typedef int pressure[3][2];
typedef int picture[100][100];
enum posture {GOOD = 0, TURTLE_NECK, WAIST_BENDING, HIP_FORWORD, LEFT_TILT, RIGHT_TILT};

float ultra_curr;
pressure weight_curr;

float ultra_REF = 2.0;                                    // 초음파 바른자세 기준값
pressure weight_REF = {{100,100},{200,200},{300,300}};    // 압력센서 바른자세 기준값

const int CollectDataNUM = 100;
int CurrDataNUM = 0;
int PostureData[100] = {-1,};

unsigned long currTime = 0;
unsigned long SitBeginTime = 0;

void setup()
{
  // camera sensor initialize
  // sensor pin map
  Serial.begin(112800);
}

void loop()
{
  // ReadSensor(ultra_curr, weight_curr);   // 센서 값을 읽어 curr변수에 저장
  // enum posture result = DataToPi(ultra_curr, whight_curr); // 센서값 라즈베리파이로 전송해 결과를 받아옴
  /* 라즈베리파이
   *  센서 값이 들어오길 기다리고, 사진을 찍는다
   *  이미지 데이터를 activation으로 하여 model에 넣어 각 class별 확률을 반환
   *  압력 센서 값과 초음파센서 값이 위에서 가장 확률이 높은 자세 범위에 들어가는지 비교
   *  판단 후 가장 확률이 높은 class를 아두이노로 전송
  */
  // if feedback == true                    // feedback값을 어떤 방법으로 조절하지?
  //   if result != GOOD LED ON
  // if (currDataNUM >= 100)
  //   currDataNUM = 0;
  //   각 class별 개수를 어플로 전달
  // postureData[currDataNUM++] = result;
  currTime = millis();  
  // sitbegintime은 앉지 않은 상태에서 계속 currTime으로 업데이트
  // 앚아있는 상태라면 업데이트를 하지 않음
  // if currTime - sitbegintime > 오랜 시간:
    // 어플로 스트레칭 추천
    // LED 점등?
  delay(100);
}
