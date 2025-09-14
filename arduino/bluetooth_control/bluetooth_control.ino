// This example code is in the Public Domain (or CC0 licensed, at your option.)
// By Evandro Copercini - 2018
//
// This example creates a bridge between Serial and Classical Bluetooth (SPP)
// and also demonstrate that SerialBT have the same functionalities of a normal Serial
// Note: Pairing is authenticated automatically by this device

#include "BluetoothSerial.h"

String device_name = "ESP32-BT-Slave";
char c;
// Check if Bluetooth is available
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Check Serial Port Profile
#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);
  pinMode(14,OUTPUT);
  pinMode(27,OUTPUT);
  digitalWrite(27,0);
  SerialBT.begin(device_name);  //Bluetooth device name
  //SerialBT.deleteAllBondedDevices(); // Uncomment this to delete paired devices; Must be called after begin
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
}

void loop() {
if (!SerialBT.isClosed() && SerialBT.connected()){
    if (Serial.available()) {
      SerialBT.write(Serial.read());
    }
    if (SerialBT.available()) {
      c = SerialBT.read();
      Serial.write(c);
      
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
  else
    digitalWrite(27,0);
  delay(20);
}
