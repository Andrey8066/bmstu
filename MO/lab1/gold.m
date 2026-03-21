function [x_min, y_min, k] = gold(func, a, b, e)
    k = 0;
    phi = (sqrt(5) - 1)/2;

    c = b - phi * (b - a);
    d = a + phi * (b - a);
    fc = func(c);
    fd = func(d);

    while abs(b-a) > e
       
        if fc >= fd
            a = c;
            c = d;
            d = a + phi * (b - a);
            fc = fd;
            fd = func(d);

        else 
            b = d;
            d = c;
            c = b - phi * (b - a);
            fd = fc;
            fc = func(c);
        end
        k= k + 1;
    end
x_min = (a+b)/2;
y_min = func(x_min);
end