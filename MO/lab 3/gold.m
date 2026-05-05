function lambda = gold(func,a, b, epsilon)
    

    phi = (sqrt(5) - 1)/2;

    c = b - phi * (b-a);
    d = a + phi * (b-a);
    fc = func(c);
    fd = func(d);

    while b-a > epsilon

        if fc > fd
            a = c;
            c = d;
            d = a + phi * (b-a);
            fc = fd;
            fd = func(d);
        else

            b = d;
            d = c;
            c = b - phi * (b-a);
            fd = fc;
            fc = func(c);

        end
    end
    lambda = (c+d)/2;
end