// 마이크 테스트 코드

const int sampleWindow = 50;  // (50 mS = 20Hz) 샘플링 시간 결정 50ms동안 센서 값을 읽음
unsigned int sample;
int sound = A0;  

void setup() {
   Serial.begin(9600);
   pinMode(sound,INPUT);      //A0핀을 input으로 설정
}

void loop() {

   unsigned long startMillis= millis();            // 시작 시간을 잰다. millis함수는 아두이노 시작 시간을 측정
   unsigned int peakToPeak = 0;                    // peak-to-peak level
 
   unsigned int signalMax = 0;
   unsigned int signalMin = 1024;
 
   while (millis() - startMillis < sampleWindow)   // 50ms 지정된 시간만큼 데이터 샘플링
   {
      sample = analogRead(sound);
      if (sample < 1024)                           // 1024 이하일 때
      {
         if (sample > signalMax)                   // 최댓값 갱신
         { signalMax = sample; }                    // 최댓값이 기존 최댓값보다 높으면 갱신
         else if (sample < signalMin)
         { signalMin = sample; }                    // 최소값 갱신
      }
   }
   peakToPeak = signalMax - signalMin;             // max - min = peak-peak 진폭 = 소리크기
  
   Serial.println(peakToPeak);           //시리얼 출력

}
