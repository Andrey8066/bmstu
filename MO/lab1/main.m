clear all
clc

fprintf("Метод брутфорса\n");
fprintf("Минимум первой функции\n");
t = tic;
[x, y, k] = bruteforce(@optimisationFunction1, -5, 5, 0.001);
pause(1);
time = toc(t);
fprintf("x = %.3f, y = %.3f, k = %d,  Время выполнения = %.3f секунд\n", x, y, k, time);
fprintf("Минимум второй функции\n");
t = tic;
[x, y, k] = bruteforce(@optimisationFunction2, 0, 8, 0.001);
pause(1);
time = toc(t);
fprintf("x = %.3f, y = %.3f, k = %d,  Время выполнения = %.3f секунд\n", x, y, k, time);
fprintf("Минимум третьей функции\n");
t = tic;
[x, y, k] = bruteforce(@optimisationFunction3, 1.5, 2, 0.001);
pause(1);
time = toc(t);
fprintf("x = %.3f, y = %.3f, k = %d,  Время выполнения = %.3f секунд\n", x, y, k, time);

fprintf("\n")

fprintf("Метод дихотомии\n");

fprintf("Минимум первой функции\n");
t = tic;
[x, y, k] = dihotom(@optimisationFunction1, -5, 5, 0.001);
pause(1);
time = toc(t);
fprintf("x = %.3f, y = %.3f, k = %d,  Время выполнения = %.3f секунд\n", x, y, k, time);

fprintf("Минимум второй функции\n");
t = tic;
[x, y, k] = dihotom(@optimisationFunction2, 0, 8, 0.001);
pause(1);
time = toc(t);
fprintf("x = %.3f, y = %.3f, k = %d,  Время выполнения = %.3f секунд\n", x, y, k, time);

fprintf("Минимум третьей функции\n");
t = tic;
[x, y, k] = dihotom(@optimisationFunction3, 1.5, 2, 0.01);
pause(1);
time = toc(t);
fprintf("x = %.3f, y = %.3f, k = %d,  Время выполнения = %.3f секунд\n", x, y, k, time);

fprintf("\n")

fprintf("Метод золотого сечения\n");

fprintf("Минимум первой функции\n");
t = tic;
[x, y, k] = gold(@optimisationFunction1, -5, 5, 0.001);
pause(1);
time = toc(t);
fprintf("x = %.3f, y = %.3f, k = %d,  Время выполнения = %.3f секунд\n", x, y, k, time);

fprintf("Минимум второй функции\n");
t = tic;
[x, y, k] = gold(@optimisationFunction2, 0, 8, 0.001);
pause(1);
time = toc(t);
fprintf("x = %.3f, y = %.3f, k = %d,  Время выполнения = %.3f секунд\n", x, y, k, time);

fprintf("Минимум третьей функции\n");
t = tic;
[x, y, k] = gold(@optimisationFunction3, 1.5, 2, 0.001);
pause(1);
time = toc(t);
fprintf("x = %.3f, y = %.3f, k = %d,  Время выполнения = %.3f секунд\n", x, y, k, time);

fprintf("\n")

fprintf("Метод фибоначи\n");

fprintf("Минимум первой функции\n");
t = tic;
[x, y, k] = fibonaci(@optimisationFunction1, -5, 5, 0.001);
pause(1);
time = toc(t);
fprintf("x = %.3f, y = %.3f, k = %d,  Время выполнения = %.3f секунд\n", x, y, k, time);

fprintf("Минимум второй функции\n");
t = tic;
[x, y, k] = fibonaci(@optimisationFunction2, 0, 8, 0.001);
pause(1);
time = toc(t);
fprintf("x = %.3f, y = %.3f, k = %d,  Время выполнения = %.3f секунд\n", x, y, k, time);

fprintf("Минимум третьей функции\n");
t = tic;
[x, y, k] = fibonaci(@optimisationFunction3, 1.5, 2, 0.001);
pause(1);
time = toc(t);
fprintf("x = %.3f, y = %.3f, k = %d,  Время выполнения = %.3f секунд\n", x, y, k, time);
