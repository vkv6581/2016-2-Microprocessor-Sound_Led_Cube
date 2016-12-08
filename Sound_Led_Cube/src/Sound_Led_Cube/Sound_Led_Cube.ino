//타이머 인터럽트 사용을 위한 라이브러리.
#include <MsTimer2.h>

//갯수 설정
const int n_reg = 6;          //레지스터 갯수 설정
const int n_height = 6;       //Led높이 설정
const int t_sample = 50;      //샘플링 시간(50ms)

//핀 설정
//센서 핀
int s_pot = A1;               //가변저항 입력 핀
int s_mic = A0;               //마이크 입력 핀
//시프트 레지스터 핀
int reg_latchpin = 2;         //latch핀
int reg_clockpin = 3;         //clock핀
int reg_mrpin = 4;            //초기화 핀 (active low)
const int reg_datapin[] = {5,6,7,8,9,10};   //데이터가 전송되는 핀

//인터럽트에서 사용할 변수
unsigned int timer_time = 0;     //인터럽트에서 증가하는 시간
unsigned int timer_LED  = 0;     //시간을 %6한 결과값
byte data_led[] = {0,0,0,0,0,0}; //Led를 출력하기 위한 byte값. shiftout함수로 전할 데이터임.

///////////////////////////////////////////////////////////////////////////////////////

//타이머 인터럽트 시간마다 실행하는 함수. LED의 깜빡임 구현.
void timer_interrupt()
{
  timer_time++;                 //시간을 계속 증가
  timer_LED = timer_time%6;     //증가하는 시간을 %6연산을 해서 따로 저장(어떤 LED를 켤지 정하기 위해서)
  //LED를 전부 초기화하고 시작 mrpin은 activelow이기때문에 low후 high로 전환
  digitalWrite(reg_mrpin,LOW);
  digitalWrite(reg_mrpin,HIGH);

  //LED라인이 켜 질 순서라면
  //시프트 레지스터에 loop에서 계산된 값 만큼의 높이를 출력시켜줌
  //시프트 레지스터에 데이터 출력을 하기 위해선 latch핀을 low로 만든 후 shiftout 후 다시 latch핀을 high로 변경
  //clock은 함수 내에서 clock핀으로 자동 생성해줌
  digitalWrite(reg_latchpin,LOW);
  shiftOut(reg_datapin[timer_LED],reg_clockpin,MSBFIRST,data_led[timer_LED]);
  digitalWrite(reg_latchpin,HIGH);
}
///////////////////////////////////////////////////////////////////////////////////////////
void setup() 
{
  //시리얼로 
  Serial.begin(9600);
  //핀 모드 설정
  pinMode(s_pot,INPUT);
  pinMode(s_mic,INPUT);
  pinMode(reg_latchpin,OUTPUT);
  pinMode(reg_clockpin,OUTPUT);
  pinMode(reg_mrpin,OUTPUT);
  for(int i=0;i<6;i++)
  { pinMode(reg_datapin[i],OUTPUT); }

  digitalWrite(reg_mrpin,HIGH);             //MR핀을 비활성화해줌

  //타이머 사용을 위한 함수
  MsTimer2::set(1,timer_interrupt);       //5ms마다 timer_함수 실행
  MsTimer2::start();
}
/////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  //가변저항의 값을 먼저 ADC로 읽어들임.
  // ADC의 기준전압을 사용하는 전압으로 바꿔줌(5V혹은 3.3V)
  analogReference(DEFAULT);
  
  //input_pot변수에 s_pot포트에서 들어오는 아날로그 저항 값을 읽어들임
  unsigned int input_pot = analogRead(s_pot);

  //////////////////////////////////////////////////////////////////////////////////////
  //마이크의 값을 읽은 후 Peak To Peak 방식으로 처리한다.

  //전송되는 데이터의 최솟값, 최댓값, 차이를 저장할 변수 선언
  unsigned int data_ptp = 0;                   //차이
  unsigned int data_mic_min = 1024;           //최소값
  unsigned int data_mic_max = 0;              //최댓값
  
  //마이크의 값을 읽어서 저장하는 변수
  unsigned int input_mic = 0;
  
  // 시작 시간을 잰다. millis() 함수가 현재시간(아두이노가 시작된 후부터 경과된 시간)을 밀리초 단위로(1/1000 초) 리턴
  unsigned long t_start = millis();

  //while문으로 지정된 시간만큼 마이크 데이터 샘플링
  while(millis() - t_start < t_sample)
  {
    unsigned int input_mic = analogRead(s_mic);
    if(input_mic < 1024)                        //1024이상의 값은 오류이므로 버림
    {
      //최댓값 최소값이 바뀔때마다 갱신한다.
      if(input_mic > data_mic_max)
      { data_mic_max = input_mic; }
      else if(input_mic < data_mic_min)
      { data_mic_min = input_mic; }
    }
  }
  data_ptp = data_mic_max - data_mic_min;       //샘플시간동안의 최댓값-최솟값을 저장한다 (peakTopeak방식의 소리의 크기)

///////////////////////////////////////////////////////////////////////////////////////////////////////
  //소리의 크기를 가변저항 입력 값으로 나눠서 출력 높이를 설정한다.(감도)
  int height = data_ptp/(input_pot/10);
  //현재 상태를 serial에 출력한다.
  Serial.print("mic:");
  Serial.print(data_ptp);
  Serial.print(" pot:");
  Serial.print(input_pot);
  Serial.print(" height:");
  Serial.println(height);
  
  //만약 높이가 최대 높이를 넘는다면 최대 높이로 바꿔준다.
  if(height > n_height) { height = n_height; }

  //옆으로 넘어가는 것을 구현하기 위해 새로운 data를 배열에 넣기 전에 data를 옆으로 한칸씩 미뤄준다
  data_led[5] = data_led[4];
  data_led[4] = data_led[3];
  data_led[3] = data_led[2];
  data_led[2] = data_led[1];
  data_led[1] = data_led[0];
  //led[0]에 높이에 따른 데이터 출력
  //출력할 시프트 데이터 byte 선언
  byte p_led = 0;
  //LED높이에 byte변수 설정
  for(int i=0 ; i<height ; i++)
  { bitSet(p_led,i+1); }
  //LED 데이터 출력.
  data_led[0] = p_led;
}
