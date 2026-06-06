function H = hesse(func, x, epsilon)

    n = length(x);

    H = zeros(n);

    g0 = gradient(func, x, epsilon);

    for j = 1:n

        e = zeros(n,1);
        e(j) = epsilon;

        g1 = gradient(func, x + e, epsilon);

        H(:,j) = (g1 - g0) / epsilon;

    end

end