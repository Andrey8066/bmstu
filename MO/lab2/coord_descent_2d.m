function [x, y] = coord_descent_2d()
    x = 0;
    y = 0;

    h = 0.1;
    tol = 1e-6;

    for k = 1:1000
        x_old = x
        y_old = y

        while true
            f0 = f(x, y);
            f1 = f(x + h, y);
            f2 = f(x - h, y);

            if f1 < f0
                x = x + h
            elseif f2 < f0
                x = x - h
            else
                break;
            end
        end

        while true
            f0 = f(x, y);
            f1 = f(x, y + h);
            f2 = f(x, y - h);

            if f1 < f0
                y = y + h
            elseif f2 < f0
                y = y - h
            else
                break;
            end
        end

        if abs(x - x_old) < tol && abs(y - y_old) < tol
            break;
        end
    end

    disp([x, y])
end


function z = f(x, y)
    z = (x - 2)^2 + (y - 3)^2;
end