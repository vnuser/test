Chương trình đọc và nhận dữ liệu từ esp32
- Cài đặt phần mềm COM ảo trong thư mục software_vitual_com
- Chương trình cân bằng ghế cho arduino Mega cần cài đặt các thư viện sau
    + Adafruit_MPU6050
    + efll (thư viện fuzzy)
- Chương trình nạp cho esp32 cần cài thêm thư viện websocket
- Trong folder python có cả source code và file.exe nằm ở đường dẫn "python\dist\get_push_data_to_matlab" để chạy chương trình lấy dữ liệu từ esp32 từ websocket và chuyển dữ liệu cho matlab thông qua giao thức UART
