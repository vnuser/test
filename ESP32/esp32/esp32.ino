#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <Wire.h>
#define I2C_BUFFER_LENGTH 64
#define I2C_DEV_ADDR 0x0A
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>


IPAddress local_IP(192, 168, 100,200);   // Địa chỉ IP tĩnh bạn muốn
IPAddress gateway(192, 168, 1, 1);      // Địa chỉ IP của router
IPAddress subnet(255, 255, 255, 0);     // Subnet mask
IPAddress primaryDNS(8, 8, 8, 8);       // DNS server (Google)

String jsonString = "";                           // create a JSON string for sending data to the client
StaticJsonDocument<200> doc;                      // create a JSON container
JsonObject object = doc.to<JsonObject>();         // create a JSON Object
StaticJsonDocument<40> doc1; // tạo đối tượng json lưu dữ liệu từ arduino nâng xe

float roll, roll2;
const float pi = 3.14159;
Adafruit_MPU6050 mpu;
Adafruit_MPU6050 mpu2;
uint32_t i = 0;
const char* ssid = "Pneumatic_Room";
const char* password = "Mechatronic_lab@A302";
float x,y,z,x2,y2,z2;
String dataFromAdruino1; // dữ liệu từ arduino nâng hạ xe
String dataEncoder; // dữ liệu encoder từ arduino nâng hạ ghế
String website = "<!DOCTYPE html>\
  <html>\
    <head>\
    <meta charset=\"UTF-8\">\
      <title>Page Title</title>\
    </head>\
    <body style='background-color: #EEEEEE;'><span style='color: #003366;'>\
      <h1 style = 'text-align: center;'>Bảng dữ liệu</h1>\
      <p>Giá trị góc nghiêng xe: <span id='v1'>-</span></p>\
      <p>Giá trị góc nghiêng ghế: <span id='v2'>-</span></p>\
      <p>Khoảng cách siêu âm  is: <span id='v3'>-</span></p>\
      <p>Giá trị encoder nâng xe is: <span id='v4'>-</span></p>\
      <p>Giá trị encoder nâng ghế is: <span id='v5'>-</span></p>\
      <p><button type='button' id='BTN_SEND_BACK'>Send info to ESP32</button></p>\
      </span>\
    </body>\
  <script>\
    var Socket;\
    document.getElementById('BTN_SEND_BACK').addEventListener('click', button_send_back);\
    function init() {\
      Socket = new WebSocket('ws://' + window.location.hostname + ':81/');\
      Socket.onmessage = function(event) {processCommand(event); }; } \
    function button_send_back() {\
      var msg = {brand: 'Gibson',type: 'Les Paul Studio',year: 2010,color: 'white'};\
      Socket.send(JSON.stringify(msg));} \
    function processCommand(event) {\
      var obj = JSON.parse(event.data);\
      document.getElementById('v1').innerHTML = obj.v1;\
      document.getElementById('v2').innerHTML = obj.v2;\
      document.getElementById('v3').innerHTML = obj.v3;\
      document.getElementById('v4').innerHTML = obj.v4;\
      document.getElementById('v5').innerHTML = obj.v5;\
      console.log(obj.v1);\
      console.log(obj.v2); } \
      window.onload = function(event) { init(); }\
      </script>\
      </html>";

int interval = 100;
unsigned long previousMillis = 0;
char temp[50];
char temp2[10];
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);



void setup() {
  // put your setup code here, to run once:
  pinMode(21,INPUT_PULLUP);
  pinMode(22,INPUT_PULLUP);
  // Cấu hình mạng địa chỉ IP của esp32
  
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS)) {
    Serial.println("Lỗi cấu hình IP tĩnh");
  }
  
  Serial.begin(115200);
  // setup MPU6050
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // try to initialize mpu2
  if(!mpu2.begin(0x69)){
    Serial.println("Failed to find MPU6050-2 chip");
    while (1) {
      delay(10);
    }
  }
  // Try to initialize mpu!
  if (!mpu.begin(0x68)) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  Serial.println("MPU6050-2 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu2.setAccelerometerRange(MPU6050_RANGE_8_G);
  
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu2.setGyroRange(MPU6050_RANGE_500_DEG);

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  mpu2.setFilterBandwidth(MPU6050_BAND_21_HZ);
  

  Serial.println("");
  delay(100);

  Serial.setDebugOutput(true);
  Wire.begin();

  temp[4] = '\n';

  
  WiFi.begin(ssid,password);
  Serial.println("Establishing connection to wifi SSID:" + String(ssid));
  while (WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print(".");
  }

  Serial.print("Connected to net work with IP address:");
  Serial.println(WiFi.localIP());
  server.on("/", []() {                               // define here wat the webserver needs to do
    server.send(200, "text/html", website);           //    -> it needs to send out the HTML string "webpage" to the client
  });
  server.begin();                                     // start server
  
  webSocket.begin();                                  // start websocket
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  // put your main code here, to run repeatedly:
  // get new sensor events with the reading
  server.handleClient();                              // Needed for the webserver to handle all clients
  webSocket.loop();                                   // Update function for the webSockets 
  dataFromAdruino1 = "";
  dataEncoder = "";
  sensors_event_t a, g, t;
  mpu.getEvent( &a, &g, &t);
  x = a.acceleration.x;
  y = a.acceleration.y;
  z = a.acceleration.z;
  float s = sqrt(y*y + z*z);
  roll = atan2(-x,s)*180/pi; // giá trị góc nghiêng xe
  waiting(50);
  // đọc dữ liệu từ MPU6050 2
  sensors_event_t a2, g2, t2;
  mpu2.getEvent( &a2, &g2, &t2);
  x2 = a2.acceleration.x;
  y2 = a2.acceleration.y;
  z2 = a2.acceleration.z;
  float s2 = sqrt(x2*x2 + z2*z2);
  roll2 = 90 - atan2(s,z2)*180/pi; // giá trị góc nghiêng ghế
  //Serial.println("Dữ liệu từ MPU6050 1");
  //Serial.println(roll);
  //Serial.println("Dữ liệu từ MPU6050 2");
  //Serial.println(roll2);
  waiting(50);
  
   // Yêu cầu dữ liệu từ arduino nâng xe
  waiting(50);
  uint8_t bytesReceived = Wire.requestFrom(I2C_DEV_ADDR,32);
  if ((bool)bytesReceived){
    Wire.readBytes(temp,bytesReceived);
    getDataFromArduino1(temp,bytesReceived);
  }
  waiting(50);

  // yêu cầu dữ liệu encoder từ arduino nâng ghế
  uint8_t bytesReceived2 = Wire.requestFrom(15,32);
  if ((bool)bytesReceived2){
    Wire.readBytes(temp2,bytesReceived2);
    getDataFromArduino2(temp2,bytesReceived2);
  }
  //Serial.println(dataFromAdruino1);
  //Serial.println(dataEncoder);
  // chuyển đổi temp thành json
  waiting(50);

  DeserializationError error2 = deserializeJson(doc1, dataFromAdruino1);

  // gửi dữ liệu đọc được từ mpu2 cho arduino cân bằng ghế
  Wire.beginTransmission(15);
  Wire.println(roll2);
  uint8_t error = Wire.endTransmission(true);
  //Serial.println("endTransmission: %u\n");
  waiting(50);


  unsigned long now = millis();                       // read out the current "time" ("millis()" gives the time in ms since the Arduino started)
  if ((unsigned long)(now - previousMillis) >= interval) { // check if "interval" ms has passed since last time the clients were updated
    //String jsonString = "";                           // create a JSON string for sending data to the client
    //StaticJsonDocument<200> doc;                      // create a JSON container
    //JsonObject object = doc.to<JsonObject>();         // create a JSON Object
    object["v1"] = roll;                    // giá trị góc nghiêng xe
    object["v2"] = roll2;                   // giá trị góc nghiêng ghế
    object["v3"] = doc1["distance2"];       // giá trị đo từ cảm biến siêu âm nằm dưới động cơ
    object["v4"] = doc1["encoder"];         // giá trị encoder từ xy lanh cân bằng xe
    object["v5"] = dataEncoder;             // giá trị encoder cân bằng ghế
    serializeJson(doc, jsonString);                   // convert JSON object to string
    Serial.println(jsonString);                       // print JSON string to console for debug purposes (you can comment this out)
    webSocket.broadcastTXT(jsonString);               // send JSON string to clients
    previousMillis = now;                             // reset previousMillis
    /*
    String str = String(z);                 // create a random value
    int str_len = str.length() + 1;                   
    char char_array[str_len];
    str.toCharArray(char_array, str_len);             // convert to char array
    webSocket.broadcastTXT(temp);               // send char_array to clients
    previousMillis = now;                             // reset previousMillis
    */
    //x++;
  }
}

void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length) {      // the parameters of this callback function are always the same -> num: id of the client who send the event, type: type of message, payload: actual data sent and length: length of payload
  switch (type) {                                     // switch on the type of information sent
    case WStype_DISCONNECTED:                         // if a client is disconnected, then type == WStype_DISCONNECTED
      Serial.println("Client " + String(num) + " disconnected");
      break;
    case WStype_CONNECTED:                            // if a client is connected, then type == WStype_CONNECTED
      Serial.println("Client " + String(num) + " connected");
      // optionally you can add code here what to do when connected
      break;
    case WStype_TEXT:                                 // if a client has sent data, then type == WStype_TEXT
      for (int i=0; i<length; i++) {                  // print received data from client
        Serial.print((char)payload[i]);
      }
      Serial.println("");
      break;
  }
}

// hàm hiển thị dữ liệu đọc từ arduino
void getDataFromArduino1(char array[50],uint8_t len){
  //Serial.println("dữ liệu nhận được từ arduino 1");
  bool flag = true;// đặt biến flag để không in ra giá trị '\n'
  for (byte i = 0; (i<len ) && flag ; i++){
    if (array[i]=='\n')
      flag = false;
    else
      //Serial.print(array[i]);
      dataFromAdruino1+= array[i];
  }
  Serial.println();
}

void getDataFromArduino2(char array[10],uint8_t len){
  //Serial.println("dữ liệu nhận được từ arduino 2");
  bool flag = true;// đặt biến flag để không in ra giá trị '\n'
  for (byte i = 0; (i<len ) && flag ; i++){
    if (array[i]=='\n')
      flag = false;
    else
      //Serial.print(array[i]);
      dataEncoder += array[i];
  }
  Serial.println();
}


void waiting (long time){
  unsigned long x = millis();
  while((millis()-x)<time){
  }
}
