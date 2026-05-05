clear;
close all;
clc;

x0      = [100; -50];
maxiter = 1000;
epsilon = 0.0001;
desc = 1000;

colors  = {'r', 'g', 'b', 'm', 'k'};
markers = {'-o', '-s', '-^', '-d', '-p'};
names   = {'Покоорд. спуск', 'Хука-Дживс', 'Обобщ. Хука-Дживс', 'Розенброк', 'Пауэлл'};

print  = 0;

% =========================================================================
%  Rosenbrock
% =========================================================================
fprintf("Rosenbrock's function :\n");

t = tic; [x, y, k, d1] = coordDescent           (@rosenbrockFunc, x0, 150, maxiter, epsilon);
fprintf("Покоорд. спуск:      x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d2] = patternSearch           (@rosenbrockFunc, x0, 150, maxiter, epsilon);
fprintf("Хука-Дживс:          x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d3] = generalizedPatternSearch(@rosenbrockFunc, x0,  10, maxiter, epsilon);
fprintf("Обобщ. Хука-Дживс:   x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d4] = rosenbrock              (@rosenbrockFunc, x0, 150, maxiter, epsilon);
fprintf("Розенброк:           x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d5] = powell                  (@rosenbrockFunc, x0, 150, maxiter, epsilon);
fprintf("Пауэлл:              x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));

if print == 1
    maxcord = 120;
[X,Y] = meshgrid(-maxcord:maxcord/desc:maxcord, -maxcord:maxcord/desc:maxcord);
Z = arrayfun(@(a,b) rosenbrockFunc([a;b]), X, Y);
figure('Name',"Rosenbrock");
contour(X, Y, Z, 60); colorbar; hold on;
title("Rosenbrock's function — траектории");
xlabel('x_1'); ylabel('x_2');
alldots = {d1,d2,d3,d4,d5};
for i = 1:5
    d = alldots{i};
    plot(d(1,:), d(2,:), markers{i}, 'Color', colors{i}, ...
         'MarkerSize', 4, 'LineWidth', 1.2, 'DisplayName', names{i});
end
legend; hold off;
end

fprintf("\n%s\n\n", repmat('-',1,70));

% =========================================================================
%  Ackley
% =========================================================================
fprintf("Ackley's function :\n");

t = tic; [x, y, k, d1] = coordDescent           (@ackleyFunc, x0, 120, maxiter, epsilon);
fprintf("Покоорд. спуск:      x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d2] = patternSearch           (@ackleyFunc, x0, 120, maxiter, epsilon);
fprintf("Хука-Дживс:          x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d3] = generalizedPatternSearch(@ackleyFunc, x0, 120, maxiter, epsilon);
fprintf("Обобщ. Хука-Дживс:   x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d4] = rosenbrock              (@ackleyFunc, x0, 100, maxiter, epsilon);
fprintf("Розенброк:           x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d5] = powell                  (@ackleyFunc, x0, 100, maxiter, epsilon);
fprintf("Пауэлл:              x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));

if print == 1
    maxcord = 120;
[X,Y] = meshgrid(-maxcord:maxcord/desc:maxcord, -maxcord:maxcord/desc:maxcord);
Z = arrayfun(@(a,b) ackleyFunc([a;b]), X, Y);
figure('Name',"Ackley");
contour(X, Y, Z, 40); colorbar; hold on;
title("Ackley's function — траектории");
xlabel('x_1'); ylabel('x_2');
alldots = {d1,d2,d3,d4,d5};
for i = 1:5
    d = alldots{i};
    plot(d(1,:), d(2,:), markers{i}, 'Color', colors{i}, ...
         'MarkerSize', 4, 'LineWidth', 1.2, 'DisplayName', names{i});
end
legend; hold off;
end

fprintf("\n%s\n\n", repmat('-',1,70));

% =========================================================================
%  Himmelblau
% =========================================================================
fprintf("Himmelblau's function :\n");

t = tic; [x, y, k, d1] = coordDescent           (@himmelblauFunc, x0, 160, maxiter, epsilon);
fprintf("Покоорд. спуск:      x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d2] = patternSearch           (@himmelblauFunc, x0, 160, maxiter, epsilon);
fprintf("Хука-Дживс:          x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d3] = generalizedPatternSearch(@himmelblauFunc, x0, 160, maxiter, epsilon);
fprintf("Обобщ. Хука-Дживс:   x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d4] = rosenbrock              (@himmelblauFunc, x0, 160, maxiter, epsilon);
fprintf("Розенброк:           x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d5] = powell                  (@himmelblauFunc, x0, 160, maxiter, epsilon);
fprintf("Пауэлл:              x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));

if print == 1
    maxcord = 120;
[X,Y] = meshgrid(-maxcord:maxcord/desc:maxcord, -maxcord:maxcord/desc:maxcord);
Z = arrayfun(@(a,b) himmelblauFunc([a;b]), X, Y);
figure('Name',"Himmelblau");
contour(X, Y, Z, 50); colorbar; hold on;
title("Himmelblau's function — траектории");
xlabel('x_1'); ylabel('x_2');
alldots = {d1,d2,d3,d4,d5};
for i = 1:5
    d = alldots{i};
    plot(d(1,:), d(2,:), markers{i}, 'Color', colors{i}, ...
         'MarkerSize', 4, 'LineWidth', 1.2, 'DisplayName', names{i});
end
legend; hold off;
end

fprintf("\n%s\n\n", repmat('-',1,70));

% =========================================================================
%  Booth
% =========================================================================
fprintf("Booth's function :\n");

t = tic; [x, y, k, d1] = coordDescent           (@boothFunc, x0, 110, maxiter, epsilon);
fprintf("Покоорд. спуск:      x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d2] = patternSearch           (@boothFunc, x0, 110, maxiter, epsilon);
fprintf("Хука-Дживс:          x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d3] = generalizedPatternSearch(@boothFunc, x0, 110, maxiter, epsilon);
fprintf("Обобщ. Хука-Дживс:   x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d4] = rosenbrock              (@boothFunc, x0, 110, maxiter, epsilon);
fprintf("Розенброк:           x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d5] = powell                  (@boothFunc, x0, 110, maxiter, epsilon);
fprintf("Пауэлл:              x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));

if print == 1
    maxcord = 1200;
[X,Y] = meshgrid(-maxcord:maxcord/desc:maxcord, -maxcord:maxcord/desc:maxcord);
Z = arrayfun(@(a,b) boothFunc([a;b]), X, Y);
figure('Name',"Booth");
contour(X, Y, Z, 40); colorbar; hold on;
title("Booth's function — траектории");
xlabel('x_1'); ylabel('x_2');
alldots = {d1,d2,d3,d4,d5};
for i = 1:5
    d = alldots{i};
    plot(d(1,:), d(2,:), markers{i}, 'Color', colors{i}, ...
         'MarkerSize', 4, 'LineWidth', 1.2, 'DisplayName', names{i});
end
legend; hold off;
end

fprintf("\n%s\n\n", repmat('-',1,70));

% =========================================================================
%  Rastrigin
% =========================================================================
fprintf("Rastrigin's function :\n");

t = tic; [x, y, k, d1] = coordDescent           (@rastriginFunc, x0, 1000, maxiter, epsilon);
fprintf("Покоорд. спуск:      x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d2] = patternSearch           (@rastriginFunc, x0,  100, maxiter, epsilon);
fprintf("Хука-Дживс:          x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d3] = generalizedPatternSearch(@rastriginFunc, x0,   50, maxiter, epsilon);
fprintf("Обобщ. Хука-Дживс:   x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d4] = rosenbrock              (@rastriginFunc, x0,   50, maxiter, epsilon);
fprintf("Розенброк:           x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d5] = powell                  (@rastriginFunc, x0,    5, maxiter, epsilon);
fprintf("Пауэлл:              x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));

if print == 1
    maxcord = 120;
[X,Y] = meshgrid(-maxcord:maxcord/desc:maxcord, -maxcord:maxcord/desc:maxcord);
Z = arrayfun(@(a,b) rastriginFunc([a;b]), X, Y);
figure('Name',"Rastrigin");
contour(X, Y, Z, 40); colorbar; hold on;
title("Rastrigin's function — траектории");
xlabel('x_1'); ylabel('x_2');
alldots = {d1,d2,d3,d4,d5};
for i = 1:5
    d = alldots{i};
    plot(d(1,:), d(2,:), markers{i}, 'Color', colors{i}, ...
         'MarkerSize', 4, 'LineWidth', 1.2, 'DisplayName', names{i});
end
legend; hold off;
end

fprintf("\n%s\n\n", repmat('-',1,70));