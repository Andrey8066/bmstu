function g = gradient(func, x, eps)
    n = length(x);
    g = zeros(n, 1);
    e = eye(n);
    for i = 1:n
        g(i) = (func(x+eps*e(:,i)) - func(x))/eps;
    end
end