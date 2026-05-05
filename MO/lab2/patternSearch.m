function [x, y, k, dots] = patternSearch(func, x0, maxlambda, maxk, epsilon)
    
    n = length(x0);
    ei = eye(n);
    dots = [[x0;func(x0)]];

    i = 0;
    k = 0;
    x = x0;
    while k < maxk

        for i = 1:n
            f = @(lambda) func(x+lambda*ei(:,i));
            x = x  + ei(:, i) * gold(f, -maxlambda, maxlambda, epsilon);
            
        end
    if norm(x - x0) <= epsilon 
       break
    else
        d = x-x0;
        f = @(lambda) func(x+lambda*d);
        x = x  + d * gold(f, -maxlambda, maxlambda, epsilon);
    end

    if func(x) >= func(x0)
        maxlambda = maxlambda/2;
        x = x0;
    else
        x0 = x;
        dots = [dots,[x;func(x)]];
    end

    k = k+1;

   end
   y = func(x);
end