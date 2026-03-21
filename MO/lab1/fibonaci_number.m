function value = fibonaci_number(n)
    if n == 1 || n == 0
        value = 1;
    else
        value = fibonaci_number(n-1)+fibonaci_number(n-2);
    end
end