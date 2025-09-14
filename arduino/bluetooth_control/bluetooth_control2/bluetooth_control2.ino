#include "BluetoothSerial.h"

// Khai báo một đối tượng BluetoothSerial
BluetoothSerial SerialBT;

void setup() {
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);
  pinMode(14,OUTPUT);
  pinMode(27,OUTPUT);
  digitalWrite(27,0);
  // Khởi tạo Serial Monitor để theo dõi dữ liệu
  Serial.begin(115200);
  Serial.println("Chuan bi khoi dong Bluetooth...");

  // Khởi động Bluetooth với một tên cụ thể
  // Tên này sẽ xuất hiện trên thiết bị của bạn khi tìm kiếm
  SerialBT.begin("ESP32_Bluetooth2");
  Serial.println("Da khoi dong Bluetooth. San sang ket noi!");
}

void loop() {
  // Kiểm tra xem có dữ liệu nào đến từ Bluetooth không
  if (SerialBT.available()) {
    // Đọc một byte dữ liệu
    char c = SerialBT.read();

    // In dữ liệu đã nhận được ra Serial Monitor
    Serial.print("Da nhan du lieu: ");
    Serial.println(c);
    if (SerialBT.available()) {
      c = SerialBT.read();
      Serial.println(c);
      
    }
    if (c == 'F'){ // Điều kiện chạy tới
      digitalWrite(12,1);
      digitalWrite(13,0);
      digitalWrite(14,0);
      Serial.println();
    }

    else if ( c == 'B'){ // Điều kiện chạy lùi
      digitalWrite(12,0);
      digitalWrite(13,1);
      digitalWrite(14,0);
      Serial.println();
    }
    else if (c == 'R'){ // Điều kiện rẽ phải
      digitalWrite(12,0);
      digitalWrite(13,0);
      digitalWrite(14,1);
      Serial.println();
    }
    else if (c == 'L'){ // Điều kiện rẽ phải
      digitalWrite(12,0);
      digitalWrite(13,1);
      digitalWrite(14,1);
      Serial.println();
    }
    else if (c == 'K'){ // Điều kiện điều khiển bluetooth
      digitalWrite(27,1);
      Serial.println();
    }

    else if (c == 'k'){ // Điều kiện điều khiển bằng joyStick
      digitalWrite(27,0);
      Serial.println();
    }
    else{ // điều kiện dừng
      digitalWrite(12,0);
      digitalWrite(13,0);
      digitalWrite(14,0);
      Serial.println();
    }
  }

  delay(20);
}