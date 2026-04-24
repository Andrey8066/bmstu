function [x, y, k] = powell(func, x0, maxlambda, maxk, epsilon)
    

    n = length(x0);
    s = eye(n);
    x = x0;
    k = 0;

    while k < maxk

        x0 = x;


        for i = 1:n
            f = @(lambda) func(x+lambda*s(:, i));
            lambda = gold(f, -maxlambda, maxlambda , epsilon);
            x = x + lambda * s(:, i);
            %k = k + 1;
        end

        d = x - x0;

        if norm(d) < epsilon
            break;
        end

        d = d/norm(d);
        f = @(lambda) func(x+lambda*d);
        lambda = gold(f, -maxlambda, maxlambda, epsilon);
        x = x + lambda * d;


        if mod(k, (n+1)) == 0 && k > 0
            s = eye(n);
        else
            s(:, 1:n-1) = s(:, 2:n);
            s(:, n) = d ;
        end


        k = k + 1;

    end
    y = func(x);
end