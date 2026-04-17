function [x, y, k] = powell(func, x0, maxlambda, maxk, epsilon)

    n = length(x0);
    s = eye(n);
    x = x0;
    k = 0;

    while k < maxk

        x_start = x;


        for i = 1:n
            lambda = gold(func, x, -maxlambda, maxlambda, s(:, i), epsilon);
            x = x + lambda * s(:, i);
            k = k + 1;
        end


        if norm(x - x_start) <= epsilon
            break;
        end


        d = x - x_start;

        if norm(d) < 1e-12
            break;
        end


        lambda = gold(func, x, -maxlambda, maxlambda, d, epsilon);
        x = x + lambda * d;


        s(:, 1:n-1) = s(:, 2:n);
        s(:, n) = d / norm(d);

        k = k + 1;

    end
    y = func(x);
end