omega = 1e2:1e5;
N = length(omega);

R = 180; Rk = 200; C = 1e-6; L = 100e-3;

A1 = zeros(N, 1); A2 = A1; A3 = A1; A4 = A1; P1 = A1; P2 = A1; P3 = A1; P4 = A1;

for i = 1:N
    A1(i) = 1/sqrt(1+(omega(i)*R*C)^2);
    P1(i) = (-atan(omega(i)*R*C))*180/pi;
    A2(i) = (omega(i)*R*C)/sqrt(1+(omega(i)*R*C)^2);
    P2(i) = (pi/2 - atan(omega(i)*R*C))*180/pi;
    A3(i) = sqrt(Rk^2 + (omega(i)*L)^2)/sqrt((R+Rk)^2+(omega(i)*L)^2);
    P3(i) = (atan(omega(i)*L/Rk) - atan(omega(i)*L/(R+Rk)))*180/pi;
    A4(i) = R/sqrt((R+Rk)^2+(omega(i)*L)^2);
    P4(i) = (-atan(omega(i)*L/R))*180/pi;
end

lw = log10(omega);

figure; plot(lw, A1, lw, A2, lw, A3, lw, A4);
title('Амплитудно-частотная характеристика');
subtitle('Вариант 2, R1 = 120 Ом, Rк = 100 Ом, Lк = 47 мГн, C = 2.2 мкФ');
ylabel('Амплитуда Ku(ω)'); xlabel('Десятичный логарифм от частоты ω');
legend('R-C цепь', 'C-R цепь', 'R-L цепь', 'L-R цепь');
grid on
figure; plot(lw, P1, lw, P2, lw, P3, lw, P4);
title('Фазо-частотная характеристика');
subtitle('Вариант 2, R1 = 120 Ом, Rк = 100 Ом, Lк = 47 мГн, C = 2.2 мкФ');
ylabel('Фаза φ(ω), град.'); xlabel('Десятичный логарифм от частоты ω');
legend('R-C цепь', 'C-R цепь', 'R-L цепь', 'L-R цепь');
grid on