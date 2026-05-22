function H = hesse(func, x0, epsilon)

    n = length(x0);

    H = zeros(n);

    for i = 1:n
        for j = 1:n

            ei = zeros(n,1);
            ej = zeros(n,1);

            ei(i) = epsilon;
            ej(j) = epsilon;

            H(i,j) = ...
                ( func(x0 + ei + ej) ...
                - func(x0 + ei - ej) ...
                - func(x0 - ei + ej) ...
                + func(x0 - ei - ej) ) ...
                / (4 * epsilon^2);

        end
    end

end