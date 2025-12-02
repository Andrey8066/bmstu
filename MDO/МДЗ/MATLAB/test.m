global inputStruct;
inputStruct.k = 10;
inputStruct.m = 15;
inputStruct.g = 9.815;
inputStruct.r0 = 3;

func = @difur;
X0 = [5;pi/6;0;0];
t = 0:0.1:20;
[~,X1] = ode45(func,t,X0);
inputStruct.m = 12;
[~,X2] = ode45(func,t,X0);
inputStruct.m = 18;
[~,X3] = ode45(func,t,X0);

max(X1(:, 1))
figure
subplot(2,1,1);
plot(t, X1(:, 1));
hold on;
plot(t, X2(:, 1));
hold on;
plot(t, X3(:, 1));
xlabel("Время, c");
ylabel("Радиус, м");
legend("m = 15", "m = 12", "m = 18");
grid("minor");
title("Зависимость радиуса от времени")

subplot(2,1,2);
plot(t, X1(:, 2));
hold on;
plot(t, X2(:, 2));
hold on;
plot(t, X3(:, 2));
xlabel("Время, c");
ylabel("Угол, радиан");
legend("m = 15", "m = 12", "m = 18");
grid("minor");
title("Зависимость угла от времени")

figure
subplot(2, 1, 1)
plot(X1(:, 1), X1(:, 3));
hold on;
plot(X2(:, 1), X2(:, 3));
hold on;
plot(X3(:, 1), X3(:, 3));
xlabel("Радиус, м");
ylabel("Радиальная скорость, м/c");
legend("m = 15", "m = 12", "m = 18");
title('Фазовая плоскость \dot r - r')

subplot(2, 1, 2)
plot(X1(:, 2), X1(:, 4));
hold on;
plot(X2(:, 2), X2(:, 4));
hold on;
plot(X3(:, 2), X3(:, 4));
xlabel("Угол, радиан");
ylabel("Угловая скорость, радиан/c");
legend("m = 15", "m = 12", "m = 18");
title('Фазовая плоскость \dot \theta - \theta');
