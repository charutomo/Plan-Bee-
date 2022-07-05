#define Blue 9
#define Green 10
#define Red 11


void setup() {
  // put your setup code here, to run once:
  pinMode(Blue, OUTPUT);
  pinMode(Green, OUTPUT);
  pinMode(Red, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  analogWrite(Blue,122);
  analogWrite(Red,0);
  delay(1000);
  analogWrite(Green,122);
  analogWrite(Blue,0);
  delay(1000);
  analogWrite(Red,122);
  analogWrite(Green,0);
}
