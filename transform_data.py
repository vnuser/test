import asyncio
import websockets
import serial
import serial.tools.list_ports
import time
import os
import json
import datetime
# khởi tạo đối tượng serial
global ser
ser = None

# hàm liệt kê tất cả các cổng COM có sẵn trên hệ thống

def list_available_port():
    ports = serial.tools.list_ports.comports()
    if not ports:
        print("không tìm thấy cổng COM nào")
        return []
    print("các cổng COM có sẵn:")
    available_ports = []
    for port, desc, hwid in sorted(ports):
        print(f" {port}: {desc}: {hwid}")
        available_ports.append(port)
    return available_ports

def set_up_serial(port_name, baud_rate):
    global ser
    try:
        # mở cổng com
        ser = serial.Serial(
            port = port_name,
            baudrate = baud_rate,
            bytesize = serial.EIGHTBITS,
            parity = serial.PARITY_NONE,
            stopbits = serial.STOPBITS_ONE,
            timeout = 1,
            write_timeout = 1
        )
        print(f"Đã mở cổng {port_name} với tốc độ {baud_rate} baud.")
        time.sleep(2) # thời gian để serial thiết lập
    except serial.SerialException as e:
        print(f"Lỗi cổng Serial: {e}")
    except Exception as e:
        print("đã xảy ra lỗi chưa xác định: {e}")
        
# hàm gửi dữ liệu cho matlab
async def data_send_to_matlab(data_to_send):
    # chuyển đổi kiểu dữ liệu sang kiểu bytes
    data_to_send = f"{data_to_send}\n"
    data_bytes = data_to_send.encode('utf-8')
    ser.write(data_bytes)
    await asyncio.sleep(0.1)
    print(f"đã gửi: {data_to_send}, {len(data_bytes)} bytes")
    

# hàm nhận dữ liệu từ webserver
async def receive_data_and_send():
    uri = "ws://" + IP +":81"
    print(f"Đang kết nối với máy chủ Websocket ESP32 tại địa chỉ {IP}")
    try:
        async with websockets.connect(uri) as websocket:
            print(f"Kết nối thành công với esp32 tại địa chỉ {uri}")
            while True:
                try:
                # Thực hiện vòng lặp liên tục nhận dữ liệu từ esp32
                    message = await websocket.recv()
                    print(f"Dữ liệu nhận được từ esp32: {message}")
                    # gửi dữ liệu nhận được cho matlab
                    await data_send_to_matlab(message)
                except websockets.exceptions.ConnectionClosedOK:
                    print("esp32 đã đóng kết nối")
                    break
                except websockets.exceptions.ConnectionClosedError as e:
                    print(f"lỗi kết nối: {e.code}, reason: {e.reason}")
                    break
                except Exception as e:
                    print(f"lỗi xảy ra trong khi nhận dữ liệu {e}")
                    break
    except ConnectionRefusedError:
        print(f"lỗi: kết nối bị từ chối. Hãy chắc chắn nhập đúng địa chỉ IP")
    except Exception as e:
        print(f"lỗi xảy ra trong quá trình kết nối: {e}")
async def main():
    await receive_data_and_send()

if __name__ == "__main__":
    list_available_port()
    Comport = input("Nhập cổng com muốn kết nối: ")
    baudrate = input("Nhập giá trị BaudRate: ")
    IP = input("Nhập địa chỉ IP của esp32: ")
    set_up_serial(Comport,baudrate)

    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\nclient đã được dừng bởi người dùng")
        print("đã ngưng truyền dữ liệu")
        # đóng cổng com
        if ser and ser.is_open:
            ser.close()
            print(f"đã đóng cổng {Comport}")
    except Exception as e:
        print(f"đã xảy ra lỗi {e}")
    os.system('pause')