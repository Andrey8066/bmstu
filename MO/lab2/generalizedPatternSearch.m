function [x, y, k] = generalizedPatternSearch(func, x0, lambda, maxk, epsilon)
    
    n = length(x0);
    ei = eye(n);

    i = 0;
    k = 0;
    x = x0;
    xs = zeros(n, 2*n);
    while k < maxk

        for i = 1:n
            xs(:, i*2-1) = x0 - lambda * ei(:, i);
            xs(:, i*2) = x0 + lambda * ei(:, i);
            k++;
        end
        x = getMinX(func, xs);
        if func(x) >= func(x0)
            lambda = lambda/2;
            if lambda < epsilon
                break;
            end
        else
            x0 = x;
        end
    end
    y = func(x);
end

function x = getMinX(func, xs)

    n = size(xs,2);
    fx = func(xs(:, 1));
    x = xs(:, 1);
    for i = 1:n
        if func(xs(:, i)) < fx
            x = xs(:, i);
            fx = func(x);
        end
    end
end