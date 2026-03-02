%(x+3)^2 +2
%[-5; 5]
function [x_min, y_min, k] = bruteforce(func, x_left, x_right, e)
    
    f_last = func(x_left);
    a = -0.1;
    delta = 0.1;
    delta_0 = 0.1;
    x = x_left;
    k = 0;
    
    while x_left < x < x_right
        k+=1;
        f = (x+3)^2 + 2;
        if f < f_last
            x_min = x;
        else
            delta = a * delta;
        end
        f_last = f;
        x = x + delta;
        if e > abs(delta)
            break
        end
        
    end
    y_min = func(x_min);

end