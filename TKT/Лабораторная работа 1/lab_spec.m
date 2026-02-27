pkg load signal
graphics_toolkit qt
function [Xk, F] = lab_spectra(Xn, n, fd, param)
    Xn = Xn(:);
    switch param 
    case 'all'
        Xn=Xn-mean(Xn);
        w = tukeywin(length(Xn), 0.05);
        Xn = Xn .* w;
    case 'nowindow'
        Xn=Xn-mean(Xn);
    case 'nomean'
        w = tukeywin(length(Xn), 0.05);
        Xn = Xn .* w;
    case 'none'
    end
    Xk = fft(Xn, n);
    Xk = abs(Xk(1:n/2+1));
    Xk = Xk(:);
    F = (0:n/2)' * (fd/n);

end

t_min = 0;
t_max = 2;

t = [0:0.01:2];
Xn = cos(2*pi*25*t)+5;

[Xk , F] = lab_spectra(Xn, 200, 100, 'all');
figure, plot(F, Xk);
xlabel('Частота, Гц');
ylabel('|X(f)|');
grid on;


N = [51, 201, 601];

X = cell(length(N), 1);
F = cell(length(N), 1);
for i = 1:length(N)
    t = [t_min:(t_max-t_min)/(N(i)-1):t_max];
    Xn = cos(2*pi*25*t)+5;
    fd = 1/(t(2)-t(1));
    [Xk , Fk] = lab_spectra(Xn, N(i), fd, 'all');
    X{i} = Xk;
    F{i} = Fk;
end

figure;
plot(F{1}, X{1}, 'LineWidth', 2);
legend("N = 51");
grid on;
hold on;
plot(F{2}, X{2}, 'LineWidth', 2);
legend("N = 201");
grid on;
hold on;
plot(F{3}, X{3}, 'LineWidth', 2);
grid on;
hold on;
legend("N = 601");
legend show;

T_max = [2, 10, 18];

X = cell(length(T_max), 1);
F = cell(length(T_max), 1);
for i = 1:length(N)
    t = [t_min:0.01:T_max(i)];
    Xn = cos(2*pi*25*t)+5;
    fd = 1/(t(2)-t(1));
    [Xk , Fk] = lab_spectra(Xn, (T_max(i)-t_min)/0.01, fd, 'all');
    X{i} = Xk;
    F{i} = Fk;
end

figure;
plot(F{1}, X{1}, 'LineWidth', 2);
grid on;
hold on;
plot(F{2}, X{2}, 'LineWidth', 2);
grid on;
hold on;
plot(F{3}, X{3}, 'LineWidth', 2);
grid on;
hold on;
legend("t_max = 2", "t_max = 10", "t_max = 18");
legend show;



Param = ['all', 'nomean'];

X = cell(length(Param), 1);
F = cell(length(Param), 1);
for i = 1:length(N)
    t = [t_min:0.01:t_max];
    Xn = cos(2*pi*25*t)+5;
    fd = 1/(t(2)-t(1));
    [Xk , Fk] = lab_spectra(Xn, 200, fd, Param(i));
    X{i} = Xk;
    F{i} = Fk;
end

figure;
plot(F{1}, X{1}, 'LineWidth', 2);
grid on;
hold on;
plot(F{2}, X{2}, 'LineWidth', 2);
grid on;
hold on;
legend("param = all", "param = nomean");
legend show;


