#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <Wire.h>
#define I2C_DEV_ADDR 0x55
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>

IPAddress local_IP(192, 168, 1, 200);   // Địa chỉ IP tĩnh bạn muốn
IPAddress gateway(192, 168, 1, 1);      // Địa chỉ IP của router
IPAddress subnet(255, 255, 255, 0);     // Subnet mask
IPAddress primaryDNS(8, 8, 8, 8);       // DNS server (Google)

Adafruit_MPU6050 mpu;

uint32_t i = 0;
const char* ssid = "Minh Triet";
const char* password = "17111977";
float z;

String website = "<!DOCTYPE html>\
  <html>\
    <head>\
      <title>Page Title</title>\
    </head>\
    <body style='background-color: #EEEEEE;'><span style='color: #003366;'>\
      <h1 style = 'text-align: center;'>Lets generate a random number</h1>\
      <p>The first random number is: <span id='rand1'>-</span></p>\
      <p>The second random number is: <span id='rand2'>-</span></p>\
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
      document.getElementById('rand1').innerHTML = obj.v1;\
      document.getElementById('rand2').innerHTML = obj.v2;\
      console.log(obj.rand1);\
      console.log(obj.rand2); } \
      window.onload = function(event) { init(); }\
      </script>\
      </html>";

int interval = 100;
unsigned long previousMillis = 0;
char temp[5];

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
unsigned long x;



void setup() {
  // put your setup code here, to run once:
  // Cấu hình mạng địa chỉ IP của esp32
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS)) {
    Serial.println("Lỗi cấu hình IP tĩnh");
  }

  Serial.begin(115200);
  // setup MPU6050
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
  sensors_event_t a, g, t;
  mpu.getEvent( &a, &g, &t);
  z = a.acceleration.z;
  Serial.println("Dữ liệu từ MPU6050");
  Serial.println(z);
  delay(100);
  server.handleClient();                              // Needed for the webserver to handle all clients
  webSocket.loop();                                   // Update function for the webSockets 

   // Yêu cầu 6 byte từ slave
   
  uint8_t bytesReceived = Wire.requestFrom(I2C_DEV_ADDR,7);
  if ((bool)bytesReceived){
    Wire.readBytes(temp,bytesReceived);
    display(temp,bytesReceived);
  }
  

  unsigned long now = millis();                       // read out the current "time" ("millis()" gives the time in ms since the Arduino started)
  if ((unsigned long)(now - previousMillis) >= interval) { // check if "interval" ms has passed since last time the clients were updated
    String jsonString = "";                           // create a JSON string for sending data to the client
    StaticJsonDocument<200> doc;                      // create a JSON container
    JsonObject object = doc.to<JsonObject>();         // create a JSON Object
    object["v1"] = temp;                    // write data into the JSON object -> I used "rand1" and "rand2" here, but you can use anything else
    object["v2"] = (String) z;
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
void display(char array[7],uint8_t len){
  Serial.println("dữ liệu nhận được từ arduino");
  bool flag = true;// đặt biến flag để không in ra giá trị '\n'
  for (byte i = 0; (i<len ) && flag ; i++){
    if (array[i]=='\n')
      flag = false;
    else
      Serial.print(array[i]);
  }
  Serial.println();
}

