function [x_min, y_min, k] = dihotom(func, a, b, e)

    k = 0;
    delta = e;

    while abs(b-a)  > e

        c = (a+b-delta)/2;
        d = (a+b+delta)/2;

        if func(c) >= func(d)
            a = c;

        else 
            b = d;

        end
        
        k= k + 1;
    end

x_min = (a+b)/2;
y_min = func(x_min);
end