function[x_min, y_min, k] = fibonaci(func, a,b,e)
k = 0;

fib = [1, 1];

n = 2;

while fib(n) < ((b-a)/e)
    n = n + 1;
    fib(n) = fib(n-1) + fib(n-2);
end

n = n-2;

c = a + fib(n)/fib(n+2) * (b - a);
d = a + fib(n+1)/fib(n+2) * (b - a);
fc = func(c);
fd = func(d);

while abs(a-b) > e

    if fc > fd
            a = c;
            c = d;
            fc = fd;
            d = a + fib(n-k+1)/fib(n+2-k) * (b - a);
            fd = func(d);

        else 
            b = d;
            d = c;
            fd = fc;
            c = a + fib(n-k)/fib(n-k+2) * (b - a);
            fc = func(c);
    end
    k = k + 1;
end

x_min = (a+b)/2;
y_min = func(x_min);
end