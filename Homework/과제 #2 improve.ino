void setup()
{
  pinMode(10, INPUT_PULLUP);  // 버튼 누름 감지 (내부 pullup저항 활용)
  pinMode(13, OUTPUT);        // LED 제어
  Serial.begin(9600);         // cnt변수 값을 모니터로 확인하기 위함
}

int cnt = 0;                  // LED 타이머 (초단위)
int PUSHCOUNT = 0;            // 버튼이 눌려있는 시간 측정 (ms단위)
int TIMECOUNT = 0;            // LED가 켜진 시간 counter (ms단위)
int btn = 1;                  // 버튼이 눌렸는지 나타내는 변수, negative PUSH

void loop()                   // 1ms마다 무한루프
{  
  int old = btn;                                // 이전 btn값 저장
  btn = digitalRead(10);                        // btn 값을 새로 읽음

  if(old == LOW && btn == LOW) PUSHCOUNT++;     // 버튼이 눌려있는 상태가 지속되면 PUSHCOUNT
  if(old == LOW && btn == HIGH){                // 버튼이 떼지면 (low to high)
    int PUSHTIME = (PUSHCOUNT/1000);            
    cnt += 5*PUSHTIME;                          // 버튼이 눌린 시간(초)의 5배만큼 cnt를 증가시키고
    PUSHCOUNT = 0;                              // PUSHCOUNT를 초기화
  }

  if (cnt > 0)                                  // LED가 켜져있을 때
  {                                           
    TIMECOUNT++;                                // LED가 켜져있으면 TIMECOUNT
    if (TIMECOUNT == 1000){                     // LED가 1초 켜져있다면
      cnt--;                                    // cnt를 1씩 감소
      TIMECOUNT = 0;
    digitalWrite(13, HIGH);						// cnt 값에 따라 LED 제어, positive ON
	}
	if (TIMECOUNT == 1) Serial.println(cnt);
  }
  else digitalWrite(13, LOW);                 

  delay(1);                                   // 1ms loop
}
