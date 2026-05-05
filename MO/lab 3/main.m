clear;
close all;
clc;

x0      = [10; -10];
maxlambda = 100;
maxiter = 1000;
epsilon = 0.001;
desc = 1000;

colors  = {'r', 'g', 'b'};
markers = {'-o', '-s', '-^'};
names   = {'Наискорейший спуск', 'Сопряженные градиенты', 'Полак-Рибьер'};

print  = 0;

% =========================================================================
%  Rosenbrock
% =========================================================================
fprintf("Rosenbrock's function :\n");

t = tic; [x, y, k, d1] = steepestDescent(@rosenbrockFunc, x0, maxlambda, maxiter, epsilon);
fprintf("Наискорейший спуск:           x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d2] = conjugateGradient(@rosenbrockFunc, x0, maxlambda, maxiter, epsilon);
fprintf("Метод сопряженных градиентов: x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d3] = polakRibiere(@rosenbrockFunc, x0, maxlambda, maxiter, epsilon);
fprintf("Метод Полака-Рибьера:         x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));

if print == 1
    maxcord = 120;
    [X,Y] = meshgrid(-maxcord:maxcord/desc:maxcord, -maxcord:maxcord/desc:maxcord);
    Z = arrayfun(@(a,b) rosenbrockFunc([a;b]), X, Y);
    figure('Name',"Rosenbrock");
    contour(X, Y, Z, 60); colorbar; hold on;
    title("Rosenbrock's function — траектории");
    xlabel('x_1'); ylabel('x_2');
    alldots = {d1,d2,d3};
    for i = 1:3
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

t = tic; [x, y, k, d1] = steepestDescent(@ackleyFunc, x0, maxlambda, maxiter, epsilon);
fprintf("Наискорейший спуск:           x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d2] = conjugateGradient(@ackleyFunc, x0, maxlambda+100, maxiter, epsilon);
fprintf("Метод сопряженных градиентов: x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d3] = polakRibiere(@ackleyFunc, x0, maxlambda+100, maxiter, epsilon);
fprintf("Метод Полака-Рибьера:         x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));

if print == 1
    maxcord = 120;
    [X,Y] = meshgrid(-maxcord:maxcord/desc:maxcord, -maxcord:maxcord/desc:maxcord);
    Z = arrayfun(@(a,b) ackleyFunc([a;b]), X, Y);
    figure('Name',"Ackley");
    contour(X, Y, Z, 40); colorbar; hold on;
    title("Ackley's function — траектории");
    xlabel('x_1'); ylabel('x_2');
    alldots = {d1,d2,d3};
    for i = 1:3
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

t = tic; [x, y, k, d1] = steepestDescent(@himmelblauFunc, x0, maxlambda, maxiter, epsilon);
fprintf("Наискорейший спуск:           x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d2] = conjugateGradient(@himmelblauFunc, x0, maxlambda, maxiter, epsilon);
fprintf("Метод сопряженных градиентов: x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d3] = polakRibiere(@himmelblauFunc, x0, maxlambda, maxiter, epsilon);
fprintf("Метод Полака-Рибьера:         x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));

if print == 1
    maxcord = 120;
    [X,Y] = meshgrid(-maxcord:maxcord/desc:maxcord, -maxcord:maxcord/desc:maxcord);
    Z = arrayfun(@(a,b) himmelblauFunc([a;b]), X, Y);
    figure('Name',"Himmelblau");
    contour(X, Y, Z, 50); colorbar; hold on;
    title("Himmelblau's function — траектории");
    xlabel('x_1'); ylabel('x_2');
    alldots = {d1,d2,d3};
    for i = 1:3
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

t = tic; [x, y, k, d1] = steepestDescent(@boothFunc, x0, maxlambda, maxiter, epsilon);
fprintf("Наискорейший спуск:           x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d2] = conjugateGradient(@boothFunc, x0, maxlambda, maxiter, epsilon);
fprintf("Метод сопряженных градиентов: x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d3] = polakRibiere(@boothFunc, x0, maxlambda, maxiter, epsilon);
fprintf("Метод Полака-Рибьера:         x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));

if print == 1
    maxcord = 120;
    [X,Y] = meshgrid(-maxcord:maxcord/desc:maxcord, -maxcord:maxcord/desc:maxcord);
    Z = arrayfun(@(a,b) boothFunc([a;b]), X, Y);
    figure('Name',"Booth");
    contour(X, Y, Z, 40); colorbar; hold on;
    title("Booth's function — траектории");
    xlabel('x_1'); ylabel('x_2');
    alldots = {d1,d2,d3};
    for i = 1:3
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

t = tic; [x, y, k, d1] = steepestDescent(@rastriginFunc, x0, maxlambda, maxiter, epsilon);
fprintf("Наискорейший спуск:           x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d2] = conjugateGradient(@rastriginFunc, x0, maxlambda, maxiter, epsilon);
fprintf("Метод сопряженных градиентов: x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));
t = tic; [x, y, k, d3] = polakRibiere(@rastriginFunc, x0, maxlambda, maxiter, epsilon);
fprintf("Метод Полака-Рибьера:         x=[%.4f,%.4f] y=%.6f k=%d t=%.4fс\n", x(1),x(2),y,k,toc(t));

if print == 1
    maxcord = 120;
    [X,Y] = meshgrid(-maxcord:maxcord/desc:maxcord, -maxcord:maxcord/desc:maxcord);
    Z = arrayfun(@(a,b) rastriginFunc([a;b]), X, Y);
    figure('Name',"Rastrigin");
    contour(X, Y, Z, 40); colorbar; hold on;
    title("Rastrigin's function — траектории");
    xlabel('x_1'); ylabel('x_2');
    alldots = {d1,d2,d3};
    for i = 1:3
        d = alldots{i};
        plot(d(1,:), d(2,:), markers{i}, 'Color', colors{i}, ...
             'MarkerSize', 4, 'LineWidth', 1.2, 'DisplayName', names{i});
    end
    legend; hold off;
end

fprintf("\n%s\n\n", repmat('-',1,70));