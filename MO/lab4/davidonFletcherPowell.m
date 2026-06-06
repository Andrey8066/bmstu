function [x, y, k, dots] = davidonFletcherPowell(func, x0, maxlambda, maxk, epsilon)
    
    n = length(x0);

    k = 0;
    x = x0;
    dots = [[x0;func(x0)]];

    g = gradient(func, x, epsilon);
    A = hesse(func, x0, epsilon)^(-1);
    M = 0;
    N = 0;

    while k < maxk
     
        g = gradient(func, x, epsilon);

        A = A + M + N;



        s = - A*g;

        f = @(lambda) func(x+lambda*s);
        x = x  + s * gold(f, 0, maxlambda, epsilon);
        dx = x- x0;
        dg = gradient(func, x, epsilon) - g;

        M = ((-A * g) * (- A * g)')/((- A * g)' * dg);
        N = ((-A * dg) * (- A * dg)')/(dg' * (- A * dg));

        x0 = x;
        dots = [dots,[x;func(x)]];
        k = k + 1;
        
        if norm(gradient(func, x, epsilon)) <= epsilon 
            break
        end
        

   end
   y = func(x);

   disp(A + M + N - hesse(func, x0, epsilon)^(-1));

end