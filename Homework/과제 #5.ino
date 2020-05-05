const int Rpin = 11;  // pin number 정의
const int Gpin = 9;
const int Bpin = 10;
const int trig = 4;
const int echo = 2;

void setup()
{
  Serial.begin(9600);
  pinMode(Rpin, OUTPUT);    // RGB LED 제어
  pinMode(Gpin, OUTPUT);  
  pinMode(Bpin, OUTPUT);
  pinMode(trig, OUTPUT);    // 초음파센서
  pinMode(echo, INPUT);
}

// 빨강, 주황, 노랑, 초록, 보라색에 대한 RGB값
int rainbow[5][3] = {{255,0,0}, {255,63,0}, \
        {255,255,0}, {0,255,0}, {127,0,255}};

int measure()                           // 초음파 센서 거리 측정 함수
{
  int duration;
  
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);              // 10us간 trigger

  duration = pulseIn(echo, HIGH);       // echo로 돌아오기까지 시간
  return duration/29/2;                 // 시간을 거리로 변환 후 반환
}

void loop()
{
  int distance = measure();             // 초음파센서 거리 측정 (cm)
  Serial.println(distance);             // 거리 출력
  int idx = distance/10;                // 거리를 10으로 나누면 배열의 인덱스
  if(idx<5){                            // 거리가 50cm 미만이면 조건에 맞는 색 표시
    analogWrite(Rpin, rainbow[idx][0]);
    analogWrite(Gpin, rainbow[idx][1]);
    analogWrite(Bpin, rainbow[idx][2]);
  }
  else{                                 // 거리가 50cm 이상이면 아무 색 표시
    analogWrite(Rpin, 48);
    analogWrite(Gpin, 10);
    analogWrite(Bpin, 36);
  }
  delay(500);
}
