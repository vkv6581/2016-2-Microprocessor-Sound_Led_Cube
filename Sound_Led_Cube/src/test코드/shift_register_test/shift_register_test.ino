// 시프트 레지스터 사용 테스트

const int N_LEDLINE = 6;                        // 레지스터 갯수 설정

int latchPin = 2;
int clockPin = 3;
int mrPin = 4;
const int LedData[] = {5,6,7,8,9,10};

byte shiftData = 0;

void setup() {
  //데이터를 내보낼 핀들을 출력으로 설정
  pinMode(latchPin,OUTPUT);   
  pinMode(clockPin,OUTPUT); 
  pinMode(mrPin,OUTPUT);
  for(int i=0; i<N_LEDLINE; i++)
  { pinMode(LedData[i],OUTPUT); }
  digitalWrite(mrPin,HIGH);
}

void loop() {

  bitSet(shiftData,5);
  
  //시프트레지스터 latch핀(STCP)를 LOW로 설정
  digitalWrite(latchPin,LOW);

  //8비트 데이터를 시프트레지스터로 보내는 shiftOUT함수 호출
  //함수 내에 클락핀에 클락을 생성하는 코드가 내장되어 있기 때문에 클락핀을 따로 조정해주지 않아도 된다.
  //MSB는 데이터를 앞부터 순서대로 전송하겠다는 의미이다.
    shiftOut(LedData[3],clockPin,MSBFIRST,shiftData);

 

  //저장이 끝나면 latch핀(STCP)를 HIGH로 설정해서 데이터 출력
  digitalWrite(latchPin,HIGH);

}
