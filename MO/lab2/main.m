clear;
close all;
clc;

x0         = [100; -50];
maxiter    = 1000;
epsilon    = 0.0001;

% =========================================================================
%  Rosenbrock's function  (минимум в [1,1])
% =========================================================================
fprintf("Rosenbrock's function :\n");

t = tic;
[x, y, k] = coordDescent(@rosenbrockFunc, x0, 150, maxiter, epsilon);
fprintf("Метод покоординатного спуска:\n  x = [%.4f, %.4f], y = %.6f, k = %d,  t = %.4f с\n", x(1), x(2), y, k, toc(t));

t = tic;
[x, y, k] = patternSearch(@rosenbrockFunc, x0, 150, maxiter, epsilon);
fprintf("Метод Хука-Дживса:\n  x = [%.4f, %.4f], y = %.6f, k = %d,  t = %.4f с\n", x(1), x(2), y, k, toc(t));

t = tic;
[x, y, k] = generalizedPatternSearch(@rosenbrockFunc, x0, 10, maxiter, epsilon);
fprintf("Обобщённый метод Хука-Дживса:\n  x = [%.4f, %.4f], y = %.6f, k = %d,  t = %.4f с\n", x(1), x(2), y, k, toc(t));

t = tic;
[x, y, k] = rosenbrock(@rosenbrockFunc, x0, 150, maxiter, epsilon);
fprintf("Метод Розенброка:\n  x = [%.4f, %.4f], y = %.6f, k = %d,  t = %.4f с\n", x(1), x(2), y, k, toc(t));

t = tic;
[x, y, k] = powell(@rosenbrockFunc, x0, 150, maxiter, epsilon);
fprintf("Метод Пауэлла:\n  x = [%.4f, %.4f], y = %.6f, k = %d,  t = %.4f с\n", x(1), x(2), y, k, toc(t));

fprintf("\n%s\n\n", repmat('-', 1, 70));

% =========================================================================
%  Ackley's function  (минимум в [0,0], значение 0)
%  Функция сильно мультимодальна — нужен широкий отрезок, но не слишком
%  большой, иначе золотое сечение застрянет в боковом минимуме.
% =========================================================================
fprintf("Ackley's function :\n");

t = tic;
[x, y, k] = coordDescent(@ackleyFunc, x0, 120, maxiter, epsilon);
fprintf("Метод покоординатного спуска:\n  x = [%.4f, %.4f], y = %.6f, k = %d,  t = %.4f с\n", x(1), x(2), y, k, toc(t));

t = tic;
[x, y, k] = patternSearch(@ackleyFunc, x0, 120, maxiter, epsilon);
fprintf("Метод Хука-Дживса:\n  x = [%.4f, %.4f], y = %.6f, k = %d,  t = %.4f с\n", x(1), x(2), y, k, toc(t));

t = tic;
[x, y, k] = generalizedPatternSearch(@ackleyFunc, x0, 120, maxiter, epsilon);
fprintf("Обобщённый метод Хука-Дживса:\n  x = [%.4f, %.4f], y = %.6f, k = %d,  t = %.4f с\n", x(1), x(2), y, k, toc(t));

t = tic;
[x, y, k] = rosenbrock(@ackleyFunc, x0, 100, maxiter, epsilon);
fprintf("Метод Розенброка:\n  x = [%.4f, %.4f], y = %.6f, k = %d,  t = %.4f с\n", x(1), x(2), y, k, toc(t));

t = tic;
[x, y, k] = powell(@ackleyFunc, x0, 100, maxiter, epsilon);
fprintf("Метод Пауэлла:\n  x = [%.4f, %.4f], y = %.6f, k = %d,  t = %.4f с\n", x(1), x(2), y, k, toc(t));

fprintf("\n%s\n\n", repmat('-', 1, 70));

% =========================================================================
%  Himmelblau's function  (четыре минимума вблизи начала координат)
%  Старт далеко — нужен широкий отрезок; метод придёт в ближайший минимум.
% =========================================================================
fprintf("Himmelblau's function :\n");

t = tic;
[x, y, k] = coordDescent(@himmelblauFunc, x0, 160, maxiter, epsilon);
fprintf("Метод покоординатного спуска:\n  x = [%.4f, %.4f], y = %.6f, k = %d,  t = %.4f с\n", x(1), x(2), y, k, toc(t));

t = tic;
[x, y, k] = patternSearch(@himmelblauFunc, x0, 160, maxiter, epsilon);
fprintf("Метод Хука-Дживса:\n  x = [%.4f, %.4f], y = %.6f, k = %d,  t = %.4f с\n", x(1), x(2), y, k, toc(t));

t = tic;
[x, y, k] = generalizedPatternSearch(@himmelblauFunc, x0, 160, maxiter, epsilon);
fprintf("Обобщённый метод Хука-Дживса:\n  x = [%.4f, %.4f], y = %.6f, k = %d,  t = %.4f с\n", x(1), x(2), y, k, toc(t));

t = tic;
[x, y, k] = rosenbrock(@himmelblauFunc, x0, 160, maxiter, epsilon);
fprintf("Метод Розенброка:\n  x = [%.4f, %.4f], y = %.6f, k = %d,  t = %.4f с\n", x(1), x(2), y, k, toc(t));

t = tic;
[x, y, k] = powell(@himmelblauFunc, x0, 160, maxiter, epsilon);
fprintf("Метод Пауэлла:\n  x = [%.4f, %.4f], y = %.6f, k = %d,  t = %.4f с\n", x(1), x(2), y, k, toc(t));

fprintf("\n%s\n\n", repmat('-', 1, 70));

% =========================================================================
%  Booth's function  (минимум в [1, 3], квадратичная — сходится легко)
% =========================================================================
fprintf("Booth's function :\n");

t = tic;
[x, y, k] = coordDescent(@boothFunc, x0, 110, maxiter, epsilon);
fprintf("Метод покоординатного спуска:\n  x = [%.4f, %.4f], y = %.6f, k = %d,  t = %.4f с\n", x(1), x(2), y, k, toc(t));

t = tic;
[x, y, k] = patternSearch(@boothFunc, x0, 110, maxiter, epsilon);
fprintf("Метод Хука-Дживса:\n  x = [%.4f, %.4f], y = %.6f, k = %d,  t = %.4f с\n", x(1), x(2), y, k, toc(t));

t = tic;
[x, y, k] = generalizedPatternSearch(@boothFunc, x0, 110, maxiter, epsilon);
fprintf("Обобщённый метод Хука-Дживса:\n  x = [%.4f, %.4f], y = %.6f, k = %d,  t = %.4f с\n", x(1), x(2), y, k, toc(t));

t = tic;
[x, y, k] = rosenbrock(@boothFunc, x0, 110, maxiter, epsilon);
fprintf("Метод Розенброка:\n  x = [%.4f, %.4f], y = %.6f, k = %d,  t = %.4f с\n", x(1), x(2), y, k, toc(t));

t = tic;
[x, y, k] = powell(@boothFunc, x0, 110, maxiter, epsilon);
fprintf("Метод Пауэлла:\n  x = [%.4f, %.4f], y = %.6f, k = %d,  t = %.4f с\n", x(1), x(2), y, k, toc(t));

fprintf("\n%s\n\n", repmat('-', 1, 70));

% =========================================================================
%  Rastrigin's function  (глобальный минимум в [0,0], значение 0)
%  Очень мультимодальна — широкий отрезок необходим, чтобы перепрыгнуть
%  через боковые ямы при большом начальном смещении.
% =========================================================================
fprintf("Rastrigin's function :\n");

t = tic;
[x, y, k] = coordDescent(@rastriginFunc, x0, 30, maxiter, epsilon);
fprintf("Метод покоординатного спуска:\n  x = [%.4f, %.4f], y = %.6f, k = %d,  t = %.4f с\n", x(1), x(2), y, k, toc(t));

t = tic;
[x, y, k] = patternSearch(@rastriginFunc, x0, 100, maxiter, epsilon);
fprintf("Метод Хука-Дживса:\n  x = [%.4f, %.4f], y = %.6f, k = %d,  t = %.4f с\n", x(1), x(2), y, k, toc(t));

t = tic;
[x, y, k] = generalizedPatternSearch(@rastriginFunc, x0, 50, maxiter, epsilon);
fprintf("Обобщённый метод Хука-Дживса:\n  x = [%.4f, %.4f], y = %.6f, k = %d,  t = %.4f с\n", x(1), x(2), y, k, toc(t));

t = tic;
[x, y, k] = rosenbrock(@rastriginFunc, x0, 50, maxiter, epsilon);
fprintf("Метод Розенброка:\n  x = [%.4f, %.4f], y = %.6f, k = %d,  t = %.4f с\n", x(1), x(2), y, k, toc(t));

t = tic;
[x, y, k] = powell(@rastriginFunc, x0, 50, maxiter, epsilon);
fprintf("Метод Пауэлла:\n  x = [%.4f, %.4f], y = %.6f, k = %d,  t = %.4f с\n", x(1), x(2), y, k, toc(t));

fprintf("\n%s\n\n", repmat('-', 1, 70));