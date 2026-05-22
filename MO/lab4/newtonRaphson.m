function [x, y, k, dots] = newtonRaphson(func, x0, maxlambda, maxk, epsilon)
    
    n = length(x0);

    k = 0;
    x = x0;
    dots = [[x0;func(x0)]];

    while k < maxk
        g = gradient(func, x, epsilon);
        
        H = hesse(func, x0, epsilon);

        s = - H^(-1)*g;

        f = @(lambda) func(x+lambda*s);
        x = x  + s * gold(f, 0, maxlambda, epsilon);
        

        if norm(gradient(func, x, epsilon)) <= epsilon 
            break
        end

        x0 = x;
        dots = [dots,[x;func(x)]];
        k = k + 1;

   end
   y = func(x);

end