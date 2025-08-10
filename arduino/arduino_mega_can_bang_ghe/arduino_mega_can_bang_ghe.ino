#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Fuzzy.h>
#define pwm_r 6
#define pwm_l 7

Adafruit_MPU6050 mpu;

uint16_t encoder_count;

float x, y , z; // Khai báo 3 biến x, y, z để lưu lại giá trị gia tốc trọng trường

float roll; // biến ghi lại giá trị góc xoay quanh trục x

float error;

float u;

bool data, data1, data2, pin24;

const float pi = 3.1415;

// khởi tạo bộ điều khiển Fuzzy
Fuzzy *fuzzy = new Fuzzy();
// khởi tạo các hàm liên thuộc đầu vào
FuzzySet *positive = new FuzzySet(2.5,4,90,90);
FuzzySet *zero = new FuzzySet(-3, -2.5, 2.5, 3);
FuzzySet *negative = new FuzzySet(-90, -90, -4, -2.5);

// khởi tạo các hàm liên thuộc đầu ra
FuzzySet *stopedOutput = new FuzzySet(-40,-30,30,40);
FuzzySet *reverseOutput = new FuzzySet(-255,-255,-30,0);
FuzzySet *forwardOutput = new FuzzySet(0,30,255,255);


void setup() {
  // put your setup code here, to run once:
  /*
  // Thiết lập các thanh ghi để đọc giá trị  encoder
  TCCR5A = 0x00;
  TCCR5B = 0x86;
  TCCR5C = 0x00;
  TCNT5H = 0x00;
  TCNT5L = 0x00;
  */
  // biến đổi đầu vào thành tỉ lệ bằng các hàm liên thuộc
  FuzzyInput *error = new FuzzyInput(1);
  error->addFuzzySet(positive);
  error->addFuzzySet(zero);
  error->addFuzzySet(negative);
  fuzzy->addFuzzyInput(error);

  // Thiết lập đầu ra cho bộ điều khiển fuzzy
  FuzzyOutput *speedOutput = new FuzzyOutput(1);
  speedOutput->addFuzzySet(stopedOutput);
  speedOutput->addFuzzySet(reverseOutput);
  speedOutput->addFuzzySet(forwardOutput);
  fuzzy->addFuzzyOutput(speedOutput);

  // Xây dựng các luật cho fuzzy
  // Luật 1
  FuzzyRuleAntecedent *ifError_p = new FuzzyRuleAntecedent();
  ifError_p->joinSingle(positive);
  FuzzyRuleConsequent *thenReverseOutput = new FuzzyRuleConsequent();
  thenReverseOutput->addOutput(reverseOutput);
  FuzzyRule *fuzzyRule1 = new FuzzyRule(1,ifError_p,thenReverseOutput);
  fuzzy->addFuzzyRule(fuzzyRule1);
  // Luật 2
  FuzzyRuleAntecedent *ifError_z = new FuzzyRuleAntecedent();
  ifError_z->joinSingle(zero);
  FuzzyRuleConsequent *thenstopedOutput = new FuzzyRuleConsequent();
  thenstopedOutput->addOutput(stopedOutput);
  FuzzyRule *fuzzyRule2 = new FuzzyRule(2,ifError_z, thenstopedOutput);
  fuzzy->addFuzzyRule(fuzzyRule2);
  //Luật 3
  FuzzyRuleAntecedent *ifError_n = new FuzzyRuleAntecedent();
  ifError_n->joinSingle(negative);
  FuzzyRuleConsequent *thenforwardOutput = new FuzzyRuleConsequent();
  thenforwardOutput->addOutput(forwardOutput);
  FuzzyRule *fuzzyRule3 = new FuzzyRule(3,ifError_n, thenforwardOutput);
  fuzzy->addFuzzyRule(fuzzyRule3);

  

  // Khai báo ngắt đọc encoder
  attachInterrupt(0, count_encoder, FALLING);


  // Khai báo các chân đầu vào và đầu ra
  pinMode(47,INPUT); 
  pinMode(49,INPUT);
  pinMode(51,INPUT);
  pinMode(24,INPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(2,INPUT_PULLUP);
  pinMode(3,INPUT_PULLUP);
  digitalWrite(4,1);
  digitalWrite(5,1);


  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);
}

void loop() {
  // put your main code here, to run repeatedly:
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  x = a.acceleration.x;
  y = a.acceleration.y;
  z = a.acceleration.z;
  float s = sqrt(x*x + y*y);
  roll = 90 - atan2(s,z)*180/pi;
  error = 0 - roll;

  Serial.print("Giá trị góc nghiêng: ");
  Serial.println(roll);
  Serial.print("error: ");
  Serial.println(error);

  
  // Thiết lập giá trị đầu vào
  fuzzy->setInput(1,error);
  // mờ hóa đầu vào
  fuzzy->fuzzify();
  // giải mờ
  u = fuzzy->defuzzify(1);
  data = digitalRead(47);
  data1 = digitalRead(51);
  data2 = digitalRead(49);
  pin24 = digitalRead(24);
  CalculateOutput(data, data1, data2, u, pin24);
}

void count_encoder(){
  if (digitalRead(3)==0)
    encoder_count +=1;
  else
    encoder_count -=1;

}

void CalculateOutput(bool data, bool data1, bool data2, float u, bool pin24){
  if(data2 == 0){
    if (data1 == 0 && data == 1){
      analogWrite(pwm_r, 255);
      analogWrite(pwm_l, 0);
    }
    else if (data1 == 1 && data == 0){
      analogWrite(pwm_l, 255);
      analogWrite(pwm_r, 0);
    }
    else
      analogWrite(pwm_r, 0);
      analogWrite(pwm_l, 0);
  }
  else{
    if (pin24 == 1){
      analogWrite(pwm_r, 255);
      analogWrite(pwm_l, 0);
    }
    else{
      if (u<0){
        analogWrite(pwm_r, abs(u));
        analogWrite(pwm_l, 0);
      }
      else if (u>0){
        analogWrite(pwm_r, 0);
        analogWrite(pwm_l, u);
      }
      else{
        analogWrite(pwm_r, 0);
        analogWrite(pwm_l, 0);
      }
    }
  }
}
