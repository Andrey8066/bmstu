function [x, y, k] = patternSearch(func, x0, maxlambda, maxk, epsilon)
    
    n = length(x0);
    ei = eye(n);

    i = 0;
    k = 0;
    x = x0;
    while k < maxk

        for i = 1:n
            x = x  + ei(:, i) * gold(func, x, -maxlambda, maxlambda, ei(:, i), epsilon);
            k++;
        end
    if norm(x - x0) <= epsilon 
       break
    else
        x = x  + (x-x0) * gold(func, x, -maxlambda, maxlambda, x-x0, epsilon);
    end

    x0 = x;

   end
   y = func(x);
end