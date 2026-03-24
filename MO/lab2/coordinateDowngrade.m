function [x, y, z, k] = coordinateDowngrade(func, X0, e )

E = eye(length(x));
while abs(Xn - X0) > e
    
    L = min(func(X + L * E));

end
end
