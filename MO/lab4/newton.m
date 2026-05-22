function [x, y, k, dots] = newton(func, x0, maxlambda, maxk, epsilon)
    
    n = length(x0);

    k = 0;
    x = x0;
    dots = [[x0;func(x0)]];

    while k < maxk
        g = gradient(func, x, epsilon);
        
        H = hesse(func, x0, epsilon);

        s = - H^(-1)*g;

        x = x  + s ;
        

        if norm(gradient(func, x, epsilon)) <= epsilon 
            break
        end

        x0 = x;
        dots = [dots,[x;func(x)]];
        k = k + 1;

   end
   y = func(x);

end