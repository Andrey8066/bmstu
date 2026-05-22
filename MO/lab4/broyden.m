function [x, y, k, dots] = broyden(func, x0, maxlambda, maxk, epsilon)
    
    n = length(x0);

    k = 0;
    x = x0;
    dots = [[x0;func(x0)]];

    A = hesse(func, x0, epsilon)^(-1);
    dA = 0;

    while k < maxk
        g = gradient(func, x, epsilon);
        
        A = A + dA;

        s = - A*g;

        f = @(lambda) func(x+lambda*s);
        x = x  + s * gold(f, 0, maxlambda, epsilon);
        dx = x- x0;
        dg = gradient(func, x, epsilon) - g;
        dA = (dx - A * dg)*((dx - A * dg)')/((dx - A * dg)'*dg);

        if norm(gradient(func, x, epsilon)) <= epsilon 
            break
        end

        x0 = x;
        dots = [dots,[x;func(x)]];
        k = k + 1;

   end
   y = func(x);

end