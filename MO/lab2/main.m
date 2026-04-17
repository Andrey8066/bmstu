
clc cls; clear; close all;

functions = {@rosenbrockFunc, @ackleyFunc, @himmelblauFunc, @boothFunc, @rastriginFunc};
functionsName = {"Rosenbrock’s function", "Ackley’s function", "Himmelblau’s function", "Booth’s function", "Rastrigin’s function"};
start_points = {[10;-10], [10;-10], [10;-10], [10;-10], [10; 10]};

maxlambda = 2;
maxiter = 1000;
epsilon = 0.0001;


for i = 1:length(functions)
    f = functions{i};

    fprintf("%s :\n", functionsName{i});
    t= tic;
    [x, y, k] = coordDescent(f, start_points{i}, maxlambda, maxiter, epsilon);
    time = toc(t);
    fprintf("Метод покоординатного спуска:\nx = [%.3f, %.3f] , y = %.3f, k = %d,  Время выполнения = %.3f секунд\n", x, y, k, time);


    t = tic;
    [x, y, k] = patternSearch(f, start_points{i}, maxlambda, maxiter, epsilon);
    time = toc(t);
    fprintf("Метод Хука-Дживса:\nx = [%.3f, %.3f], y = %.3f, k = %d,  Время выполнения = %.3f секунд\n", x, y, k, time);


    t = tic;
    [x, y, k] = generalizedPatternSearch(f, start_points{i}, maxlambda, maxiter, epsilon);
    time = toc(t);
    fprintf("Обобщенный метод Хука-Дживса :\nx = [%.3f, %.3f], y = %.3f, k = %d,  Время выполнения = %.3f секунд\n", x, y, k, time);


    t = tic;
    [x, y, k] = rosenbrock(f, start_points{i}, maxlambda, maxiter, epsilon);
    time = toc(t);
    fprintf("Метод Розенброка :\nx = [%.3f, %.3f], y = %.3f, k = %d,  Время выполнения = %.3f секунд\n", x, y, k, time);


    t = tic;
    [x, y, k] = powell(f, start_points{i}, maxlambda, maxiter, epsilon);
    time = toc(t);
    fprintf("Метод Пауэлла :\nx = [%.3f, %.3f], y = %.3f, k = %d,  Время выполнения = %.3f секунд\n", x, y, k, time);


    disp("\n ---------------------------------------------------------------------- \n")
end