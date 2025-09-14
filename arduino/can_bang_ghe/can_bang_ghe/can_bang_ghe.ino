// Basic demo for accelerometer readings from Adafruit MPU6050
#define mode_button 49
#define PWMR 6
#define PWML 7
#define UP 47
#define DOWN 51
#define encoderA 2
#define encoderB 3
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
const float pi = 3.14;
float roll, Acc_x, Acc_y, Acc_z, s;
float volatile encodervalue;
float rotate_angle;
String mpu;

void setup(void) {
  Wire.begin(15);
  pinMode(mode_button,INPUT);
  pinMode(PWMR,OUTPUT);
  pinMode(PWML,OUTPUT);
  pinMode(UP,INPUT_PULLUP);
  pinMode(DOWN,INPUT_PULLUP);
  pinMode(5,OUTPUT);
  pinMode(4,OUTPUT);
  digitalWrite(5,1);
  digitalWrite(4,1);
  pinMode(encoderA,INPUT_PULLUP);
  pinMode(encoderB,INPUT_PULLUP);
  attachInterrupt(0,countEncoder,FALLING);
  Serial.begin(115200);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("");
  delay(100);
}

void loop() {
  /* Get new sensor events with the readings */
  if (digitalRead(mode_button)==0){
    if (digitalRead(DOWN)==0 && digitalRead(UP)==1){
      analogWrite(PWMR,255);
      analogWrite(PWML,0);
    }
    else if (digitalRead(DOWN)==1 && digitalRead(UP)==0){
      analogWrite(PWMR,0);
      analogWrite(PWML,255);
    }
    else{
      analogWrite(PWMR,0);
      analogWrite(PWML,0);
    }
  }
  rotate_angle = encodervalue*360/100;
  /* Print out the values */
  Serial.print("giá trị góc nghiêng: ");
  Serial.println(mpu);
  waiting(100);
}

void countEncoder(){
  if(digitalRead(encoderB)==1){
    encodervalue--;
  }
  else{
    encodervalue++;
  }
}

void waiting (long time){
  unsigned long x = millis();
  while((millis()-x)<time){
  }
}

void receiveEvent(int howMany){
  while (1 < Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
}

void requestEvent(){
  String a = (String) rotate_angle + '\n';
  Wire.print(a);
}