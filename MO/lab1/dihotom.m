function [x_min, y_min, k] = dihotom(func, a, b, e)
    k = 0;
    while abs(b-a) -(2*e) > 10 ^ (-15)
        c = (a+b-2*e)/2;
        d = (a+b+2*e)/2;
        if func(c) >= func(d)
            a = c;
        else 
            b = d;
        end
        k+=1;
    end
x_min = (a+b)/2;
y_min = func(x_min);
end