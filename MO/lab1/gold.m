function [x_min, y_min, k] = gold(func, a, b, e)
    k = 0;
    phi = (sqrt(5) - 1)/2;

    c = a + phi * (b - a);
    d = b - phi * (b - a);

    while abs(b-a) -(2*e) > 10 ^ (-15)
       
        if func(c) >= func(d)
            a = c;
            b = b;
            c = d;
            d = b - phi * (b - a);

        else 
            a = a;
            b = d;
            d = c;
            c = a + phi * (b - a);
        end
        k+=1;
    end
x_min = (a+b)/2;
y_min = func(x_min);
end