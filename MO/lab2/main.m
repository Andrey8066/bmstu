
clc cls; clear; 

functions = {@rosenbrockFunc, @ackleyFunc, @himmelblauFunc, @boothFunc, @rastriginFunc};
functionsName = {"Rosenbrock’s function", "Ackley’s function", "Himmelblau’s function", "Booth’s function", "Rastrigin’s function"};
for i = 1:length(functions)
    f = functions{i};

t = tic;
[x, y, k] = coordDescent(f, [0;0], 1, 1000, 0.0001);
time = toc(t);
fprintf("%s, метод покоординатного спуска: x = [%.3f, %.3f] , y = %.3f, k = %d,  Время выполнения = %.3f миллисекунд\n", functionsName{i}, x, y, k, time);


t = tic;
[x, y, k] = patternSearch(f, [0;0], 1, 1000, 0.0001);
time = toc(t);
fprintf("%s, метод Хука-Дживса:x = [%.3f, %.3f], y = %.3f, k = %d,  Время выполнения = %.3f миллисекунд\n", functionsName{i}, x, y, k, time);


t = tic;
[x, y, k] = generalizedPatternSearch(f, [0;0], 1, 100000, 0.0001);
time = toc(t);
fprintf("%s, обобщенный метод Хука-Дживса :x = [%.3f, %.3f], y = %.3f, k = %d,  Время выполнения = %.3f миллисекунд\n", functionsName{i}, x, y, k, time);


t = tic;
[x, y, k] = rosenbrock(f, [0;0], 1, 10000, 0.0001);
time = toc(t);
fprintf("%s, метод Розенброка :x = [%.3f, %.3f], y = %.3f, k = %d,  Время выполнения = %.3f миллисекунд\n", functionsName{i}, x, y, k, time);


t = tic;
[x, y, k] = powell(f, [0;0], 1, 10000, 0.0001);
time = toc(t);
fprintf("%s, метод Пауэлла :x = [%.3f, %.3f], y = %.3f, k = %d,  Время выполнения = %.3f миллисекунд\n", functionsName{i}, x, y, k, time);

end