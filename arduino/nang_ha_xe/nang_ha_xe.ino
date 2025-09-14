#include<Wire.h>
#define I2C_BUFFER_LENGTH 64
#include<ArduinoJson.h>
#include<String.h>
#define timeOut 5000
#define encoderA 2
#define encoderB 3
#define PWMR 6
#define PWML 7
#define UP 38
#define DOWN 40
#define ENR 22
#define ENL 24
#define SS 26 //cảm biến hồng ngoại
float distance1, distance2, distance3, distance4, distance5, distance6; // tương ứng với vị trí gần cuối bánh đai bên trái, gần động cơ, gần cảm biến hồng ngoại, cuối bánh đai bên phải
byte trigPin1 = 8, echoPin1 = 9, trigPin2 = 10, echoPin2 = 11, trigPin3 = 4, echoPin3 = 5, trigPin4 = 35, echoPin4 = 36, trigPin5 = 44, echoPin5 = 42, trigPin6 = 48, echoPin6 = 50;
volatile int enValue;
bool infrared_ss;
// tạo biến string lưu giá trị tất cả các dữ liệu
String Datastring;
void setup() {
  // put your setup code here, to run once:
  Wire.begin(10);
  Wire.onRequest(requestEvent);
  Serial.begin(115200);
  pinMode(46,OUTPUT);
  digitalWrite(46,1);
  pinMode(trigPin1,OUTPUT);
  pinMode(trigPin2,OUTPUT);
  pinMode(trigPin3,OUTPUT);
  pinMode(trigPin4,OUTPUT);
  pinMode(trigPin5,OUTPUT);
  pinMode(trigPin6,OUTPUT);
  pinMode(echoPin1,INPUT);
  pinMode(echoPin2,INPUT);
  pinMode(echoPin3,INPUT);
  pinMode(echoPin4,INPUT);
  pinMode(echoPin5,INPUT);
  pinMode(echoPin6,INPUT);
  pinMode(encoderA, INPUT_PULLUP);
  pinMode(encoderB, INPUT_PULLUP);
  pinMode(PWMR,OUTPUT);
  pinMode(PWML,OUTPUT);
  pinMode(UP,INPUT_PULLUP);
  pinMode(DOWN,INPUT_PULLUP);
  pinMode(ENR, OUTPUT);
  pinMode(ENL, OUTPUT);
  digitalWrite(ENR, 1);
  digitalWrite(ENL, 1);
  pinMode(SS, INPUT);
  attachInterrupt(0,readEncoder,RISING);
}

void loop() {
  // put your main code here, to run repeatedly:
  // read data from infrared sensor
  infrared_ss = digitalRead(SS);
  distance1 = readUltraSonic(trigPin1, echoPin1);
  distance2 = readUltraSonic(trigPin2, echoPin2);
  distance3 = readUltraSonic(trigPin3, echoPin3);
  distance4 = readUltraSonic(trigPin4, echoPin4);
  distance5 = readUltraSonic(trigPin5, echoPin5);
  distance6 = readUltraSonic(trigPin6, echoPin6);
  Serial.println(distance6);
  Datastring = createJsonString(distance2, enValue);
  //Serial.println(Datastring);
  controlCylynder();
  waiting(100);
}
float readUltraSonic(byte trigPin, byte echoPin){
  long duration, distance;
  digitalWrite(trigPin,0);
  delayMicroseconds(2);
  digitalWrite(trigPin,1);
  delayMicroseconds(10);
  digitalWrite(trigPin,0);
  duration = pulseIn(echoPin,HIGH,timeOut);
  distance = duration/29.1/2;
  return distance;
}

void readEncoder(){
  if(digitalRead(encoderB)==1){
    enValue+=1;
  }
  else{
    enValue-=1;
  }
}

void controlCylynder(){
  if(digitalRead(UP)==0){// đọc tín hiệu tại chân 38
    analogWrite(PWMR,255);
    analogWrite(PWML,0);
  }
  else if(digitalRead(DOWN)==0){// đọc tín hiệu tại chân 40
    analogWrite(PWMR,0);
    analogWrite(PWML,255);
  }
  else{
    analogWrite(PWMR,0);
    analogWrite(PWML,0);
  }
}
void waiting (long time){
  unsigned long x = millis();
  while((millis()-x)<time){

  }
}

void requestEvent(){
  Wire.println(Datastring);
}
// hàm tạo đối tượng Json
String createJsonString(float distance2, int encoder){
  const size_t capacity = JSON_OBJECT_SIZE(2);
  StaticJsonDocument<capacity> doc;
  // gán giá trị cho các key
  doc["distance2"] = distance2;
  doc["encoder"] = encoder;
  String jsonString;
  serializeJson(doc,jsonString);
  jsonString = jsonString+'\n';
  return jsonString;
}
