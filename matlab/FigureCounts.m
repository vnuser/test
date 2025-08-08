% Chương trình đếm số lượng cửa sổ được tạo ra
function count = FigureCounts(action)
    persistent objCount;
    if isempty(objCount)
        objCount = 0;
    end
    if nargin >0 && strcmp(action,'plus')
        objCount = objCount + 1;
    elseif nargin > 0 && strcmp(action,'sub')
        objCount = objCount - 1;
    end
    count = objCount;
end
