function [x, y, k, dots] = polakRibiere(func, x0, maxlambda, maxk, epsilon)
    
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
        
        w = max(0, g'*(g - g0)/(g0'*s));
        s= -g + w*s;

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