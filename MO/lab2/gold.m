function lambda = gold(func, x0, a, b, ei, epsilon)
    f = @(lambda) func(x0+lambda*ei);

    phi = (sqrt(5) - 1)/2;
    lambda = b;
    
    fa = f(a);
    fb = f(b);

    c = b - phi * (b-a);
    d = a + phi * (b-a);
    fc = f(c);
    fd = f(d);

    

    while b-a > epsilon

        if fc >= fd
            a = c;
            c = d;
            d = a + phi * (b-a);
            fc = fd;
            fd = f(d);
        else

            b = d;
            d = c;
            c = b - phi * (b-a);
            fd = fc;
            fc = f(c);

        end

        lambda = (c+d)/2;

    end
end