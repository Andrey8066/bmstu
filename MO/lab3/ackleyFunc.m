function y = ackleyFunc(x)

    n = length(x);

    sum1 = sum(x.^2);
    sum2 = sum(cos(2*pi*x));

    term1 = -20 * exp(-0.2 * sqrt(sum1 / n));
    term2 = -exp(sum2 / n);

    y = term1 + term2 + 20 + exp(1);

end