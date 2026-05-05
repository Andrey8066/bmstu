function [x, y, k, dots] = conjugateGradient(func, x0, maxlambda, maxk, epsilon)
    
    n = length(x0);

    k = 0;
    x = x0;
    dots = [[x0;func(x0)]];

    g = gradient(func, x, epsilon);
    s= -g;
    f = @(lambda) func(x+lambda*s);
    x = x  + s * gold(f, 0, maxlambda, epsilon);

    dots = [dots,[x;func(x)]];
    k = k + 1;
    

    while k < maxk
        g0 =  g;
        g = gradient(func, x, epsilon);
        
        if mod(k, n) == 0 && k != 0
            s = -g;
        else
            w = norm(g)^2/norm(g0)^2;
            s= -g + w*s;
        end

        f = @(lambda) func(x+lambda*s);
        x = x  + s * gold(f, 0, maxlambda, epsilon);

        if norm(g) <= epsilon 
            break
        end
           

        x0 = x;
        dots = [dots,[x;func(x)]];
        k = k + 1;

   end
   y = func(x);
end