function [x, y, k] = coordDescent(func, x0, maxlambda, maxk, epsilon)
    
    n = length(x0);
    ei = eye(n);

    k = 0;
    x = x0;

    while k < maxk

        for i = 1:n
            f = @(lambda) func(x+lambda*ei(:, i));
            x = x  + ei(:, i) * gold(f, -maxlambda, maxlambda, epsilon);
            
        end
        
        if norm(x - x0) <= epsilon 
            break
        end

        x0 = x;
        k = k + 1;

   end
   y = func(x);
end