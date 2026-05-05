function [x, y, k, dots] = rosenbrock(func, x0, maxlambda, maxk, epsilon)
    

    n = length(x0);
    s = eye(n);
    ei = eye(n);
    dots = [[x0;func(x0)]];

    i = 0;
    k = 0;
    x = x0;
    lambda = zeros(1, n);
    while k < maxk
        for i = 1:n
            f = @(lambda) func(x+lambda*s(:, i));
            lambda(i) = gold(f, -maxlambda, maxlambda, epsilon);
            x = x  + s(:, i) * lambda(i);

        end
    if norm(x - x0) <= epsilon 
       break
    else
        
        a = zeros(n, n);
        b = zeros(n, n);
        
        for i = 1:n
            if lambda(i) == 0
                a(:, i) = s(:, i);
            else
                for j = i : n
                    a(:, i) = a(:, i) + s(:, j)*lambda(j);
                end
            end
        end
        for i = 1:n
            b(:, i) = a(:, i);
            for j = 1 : i-1
                b(:, i) = b(:,i) -  (a(:,  i)' * s(:, j)) * s(:, j);
            end
                s(:, i) = b(:, i) / norm(b(:, i));
        end
    end

    k = k+1;
    x0 = x;
    dots = [dots,[x;func(x)]];

   end
   y = func(x);
end