function [x, y, k] = generalizedPatternSearch(func, x0, lambda, maxk, epsilon)
    
    n = length(x0);
    ei = eye(n);

    i = 0;
    k = 0;
    x = x0;
    xs = zeros(n, 2*n+1);
    while k < maxk

        xs(:, 1) = x;
        for i = 1:n
            xs(:, i*2) = x - lambda * ei(:, i);
            xs(:, i*2+1) = x + lambda * ei(:, i);
            
        end
        x = getMinX(func, xs);


        if func(x) >= func(x0)
            if lambda < epsilon
                break;
            end
            lambda = lambda/2;
            
        else
            x0 = x;
        end
        k = k + 1;
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