% Class tạo đối tượng figure
classdef class_figures < handle
    properties
        % Tạo các thuộc tính của cửa sổ hiển thị
        Figure
        Axes
        Plot
        Count
        XData = zeros(1,20)
        YData = zeros(1,20)
        all_data = zeros(1,50000); 
    end
    methods(Static)
        function count = Getcount()
            count = FigureCounts();
        end
    end
    methods
        function obj = class_figures(figureTitle)            %Contructor để khởi tạo cửa sổ đồ thị
            obj.Figure = figure;
            if nargin > 0 
                set(obj.Figure,'Name', figureTitle,'NumberTitle','off');
            end
            set(obj.Figure, 'CloseRequestFcn', @(src, event)close(obj));
            %Khởi tạo trục và đồ thị
            obj.Axes = gca;
            obj.Plot = plot(obj.Axes,NaN,NaN);
            xlabel(obj.Axes,'Thời gian');
            ylabel(obj.Axes,'Giá trị');
            title(obj.Axes,"Dữ liệu thời gian thực");
            grid(obj.Axes,'on');
            FigureCounts('plus');
        end

        function updateData(obj, newX, newY)
      
            if newX > 20
                % Dịch chuyển dữ liệu cũ sang trái
                obj.XData(1:end-1) = obj.XData(2:end);
                obj.YData(1:end-1) = obj.YData(2:end);
                % Thêm dữ liệu mới vào cuối mảng
                obj.XData(end) = newX;
                obj.YData(end) = newY;
            else
                obj.XData(newX) = newX;
                obj.YData(newX) = newY;
            end
            
            obj.all_data(newX) = newY;
           
            % Cập nhật dữ liệu cho hàm Plot    
            if ishandle(obj.Figure)
                if newX <= 20
                    disp('<20');
                    disp(newX);
                    set(obj.Plot,'XData',obj.XData(1:newX),'YData',obj.YData(1:newX));
                else
                    set(obj.Plot,'XData',obj.XData,'YData',obj.YData);
                end
         
                % Điều chỉnh giới hạn Trục x
                if newX > 20
                
                    xlim(obj.Axes,[obj.XData(1) obj.XData(end)]);
                else
                    xlim(obj.Axes,[obj.XData(1) 20]);
                end
                % Điều chỉnh giới hạn Trục Y
                currentYData = obj.YData(obj.YData~=0);
                if ~isempty(currentYData)
                    ylim(obj.Axes,[min(obj.YData)-2, max(obj.YData)+2]);
                end
                drawnow limitrate;
            end
        end
        
        function close(obj)
            % Phương thức để đóng cửa sổ
            if ishandle(obj.Figure)
                disp('delete');
                FigureCounts('sub');
                set(obj.Figure, 'CloseRequestFcn', 'closereq');
                close(obj.Figure);
            end
        end
        function delete(obj)
            delete(obj.Figure);
            disp('Đã xóa');
        end
    end
end

