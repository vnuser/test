clear all;
% Cấu hình cho cổng COM
disp('Các cổng COM có sẵn trên hệ thống')
ports = serialportlist("all");
disp(ports);
%comPort = input('Nhập cổng Com bạn muốn kết nối: ','s');
%baudRate = input('Nhập tốc độ baudrate: ');
% Chuẩn bị cổng Serial
disp('MATLAB: đang thiết lập cổng Serial...');

% Đóng và xóa bất kì cổng Serial cũ nào đang mở

if ~isempty(instrfind('Port','COM2'))
    fclose(instrfind('Port','COM2'));
    delete(instrfind('Port','COM2'));
end

% Tạo đối tượng serialport mới
s = serialport("COM2",115200);
% Thiết lập kí tự kết thúc dòng cho việc đọc và ghi
% "LS" là kí tự '\n'
configureTerminator(s,"LF");

%disp(['MATLAB: Đã mở cổng ', comPort, ' với tốc độ ', num2str(baudRate), ' baud.']);
disp('MATLAB: Đang chờ dữ liệu từ Python...');

% Khởi tạo chỉ số của trục X
 dataIndex = 1;

% Khởi tạo các cửa sổ hiện thị dữ liệu
f1 = class_figures('c1');
f2 = class_figures('c2');

% Đọc và hiển thị dữ liệu

while true
    try
        if (class_figures.Getcount()>0)
            disp(class_figures.Getcount());
            receivedData = readline(s);
            All_Data = strtrim(receivedData);
            disp(All_Data);
            matlab_struct = jsondecode(All_Data);
            V1 = strtrim(matlab_struct.v1);
            D2 = matlab_struct.v2;
            D1 = str2double(V1);
            f1.updateData(dataIndex,D1);
            f2.updateData(dataIndex,D2);
            dataIndex = dataIndex + 1;
            pause(0.01);
        else
            delete(f1);
            delete(f2);
            break;
        end
    catch ME
        disp(['Lỗi khi đọc dữ liệu', ME.message]);
        clear s;    
        break;
    end
end