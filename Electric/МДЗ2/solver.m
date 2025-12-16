%Константы

U1n = 220;
Pn = 22000;
nn = 716;
lambdak = 3;
etan = 0.87;
cosfn = 0.82;
E2n = 102;
I2n = 140;

t = 0.8;
q1 = 0.85;
q2 = 3;
h1 = 1.05;
h3 = 0.25;

p = 60 * 50 / nn;
fprintf("Число пар полюсов: %d\n", p);
fprintf("Число пар полюсов: %.0f\n", p);

n0 = 60 * 50 / floor(p);
fprintf("Частота вращения магнитного поля: %d\n", n0);

Mn = 9.55 * Pn * etan / nn;
fprintf("Номинальный момент двигателя: %d\n", Mn);

sn = (n0 - nn) / nn;
fprintf("Скольжение в номинальном режиме: %d\n", sn);

Mmax = Mn * lambdak;
fprintf("Максимальный момент двигателя: %d\n", Mmax);

skr = sn * (lambdak + sqrt(lambdak^2  - 1));
nkr = n0 * (1 - skr);
fprintf("Критическое скольжение: %d\n", skr);
fprintf("Критическая частота вращения: %d\n", nkr);

omega0 = 2 * pi / 60 * n0;
fprintf("Угловая частота вращения: %d\n", omega0);

R2 = Mn * omega0 * sn /(3 * I2n^2);
fprintf("Активное сопротивление фазы обмотки: %d\n", R2);

Md = Mn * t;
fprintf("Заданный момент нагрузки: %d\n", Md);

lambdad = Mmax/Md;
fprintf("Коэффициент нагрузки: %d\n", lambdad);


sd = skr / (lambdad + sqrt(lambdad^2  - 1));
nd = n0 * (1 - sd);
fprintf("Скольжение для заданного момента: %d\n", sd);
fprintf("Частота вращения для заданного момента: %d\n", nd);


S = sort([0:0.01:1 sd sn skr]);
N = [n0*(1-S)];
M = 2*Mmax./(S ./ skr + skr./S);
I2 = sqrt(M.*omega0.*S./(3 * R2));


figure, 
plot(M, N, "r", 'LineWidth', 2);
hold on;
plot(I2, N, "g", 'LineWidth', 2);
grid on;
legend("n(M)", "n(I2)");
ylabel("Скорость вращения ротора n, об/мин");
xlabel("Момент на валу М, Нм  Ток ротора I2, A");
xline(Mn, '--b', 'M_н','HandleVisibility','off');
xline(Mmax, '--b', 'M_{max}', 'HandleVisibility','off');
xline(Md, '--b', 'M_D', 'HandleVisibility','off');
xline(M(length(M)), '--b', 'M_{пуск}', 'HandleVisibility','off');
xline(I2n, '--b', 'I_{2н}', 'HandleVisibility','off');
xline(I2(find(M == Md, 1)), '--b', 'I_{2D}', 'HandleVisibility','off');
xline(I2(find(N == nkr, 1)), '--b', 'I_{2кр}', 'HandleVisibility','off');
xline(I2(length(I2)), '--b', 'I_{2пуск}', 'HandleVisibility','off');
title("Естественная характеристика двигателя")

U1dot = U1n * q1;
Mmaxdot = Mmax*q1^2;
skrdot = skr;
nkrdot = nkr;

fprintf("Уменьшенное напряжение : %d\n", U1dot);
fprintf("Уменьшенный максимальный момент: %d\n", Mmaxdot);
fprintf("Критическое скольжение: %d\n", skrdot);
fprintf("Критическая частота вращения: %d\n", nkrdot);

Sdot = sort([0:0.01:1 sd]);
Ndot = [n0*(1-Sdot)];
Mdot = 2*Mmaxdot./(Sdot ./ skrdot + skrdot./Sdot);
%M = 2*Mmax./(Sdot ./ skr + skr./Sdot);


figure,
plot(M, N, "r", 'LineWidth', 2);
hold on;
plot(Mdot, Ndot, "g", 'LineWidth', 2);
grid on;
legend("n(M)", "n(M`)");
ylabel("Скорость вращения ротора n, об/мин");
xlabel("Момент на валу М, Нм");
xline(Mmax, '--b', 'M_{max}', 'HandleVisibility','off');
xline(Mmaxdot, '--b', 'M`_{max}', 'HandleVisibility','off');
xline(Md, '--b', 'M`_D = M_D', 'HandleVisibility','off');
title("Естественная и искусственная механичесеие характеристики двигателя при изменении U1")