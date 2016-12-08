//가변저항 테스트 코드

int pot = A1;                                    //가변저항 입력

void setup() {
    Serial.begin(9600);
    pinMode(pot,INPUT);
}

void loop() {
    unsigned int sensitive = analogRead(pot);  // 가변저항의 값을 감도로 받음.
    Serial.println(sensitive);                 //시리얼 출력

}
