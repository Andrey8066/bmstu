function y = rastriginFunc(x)
    y = 10*length(x) + (x(1)^2 - 10*cos(2*pi*x(1))) + (x(2)^2 - 10*cos(2*pi*x(2)));
end