// PSEUDO CODE for Data Collecting

typedef int pressure[3][2];
typedef int picture[100][100];
enum posture {GOOD, TURTLE_NECK, WAIST_BENDING, HIP_FORWORD, LEFT_TILT, RIGHT_TILT};

float ultra_curr;
pressure weight_curr;

float ultra_REF = 2.0;                                    // 초음파 바른자세 기준값
pressure weight_REF = {{100,100},{200,200},{300,300}};    // 압력센서 바른자세 기준값

void setup()
{
  // camera sensor initialize
  // sensor pin map
  Serial.begin(112800);
}

void loop()
{
  // ReadSensor(ultra_curr, weight_curr);
  // TakePicture();
  // PrintSensor();
  // 
  delay(100);
}
