funtion x = rk4(odefun, t_0, X_0)

lt = length(t_0);
x(1;1) = X_0;

for i = 2:1:lt
    h = t_vec(i) - t_vec(i-1);
    K1 = odefun(t_vec(i-1), x(:,i-1));
    K2 = odefun(t_vec(i-1) + 0.5 * h, x(:,i-1));
    K3 = odefun(t_vec(i-1) + 0.5 * h, x(:,i-1));
    K4 = odefun(t_vec(i-1) + h, x(:,i-1) + h * K3);
    
    x(i, : ) = x(:, i-1) + h/6 * (K1 + 2*K2 + 2*K3 +2 * K4);
end
end