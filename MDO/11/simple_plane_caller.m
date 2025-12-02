clc 
clear all
close all force

odefun = @simple_plane_ode;

global g;
g = 9.815;

t_vec = 0 : 0.01 : 300;
X0 = [0 0 0 0.001 0.001 0.001]';
X = rk4(odefun, t_vec, X0);


V = sqrt(X(:,4) ^ 2 +X(:,5) ^ 2 + X(:,6) ^ 2);
Vg = sqrt(X(:,4) ^ 2 + X(:,6) ^2 );

figure
clf
subplot(2,2,[1,2]);
plot3(X(:,1), -X(:,3), X(:,2), 'LineWidth', 2);
grid on;
grid  minor;
xlabel('X, m');
ylabel('Y, m');
zlabel('Z, m');


subplot(2,2,3)
hold on;
plot(t_vec, V, '-r', 'LineWidth', 2);
plot(t_vec, Vg, '-r', 'LineWidth', 2);
grid on;
grid  minor;
xlabel('Time, s');
ylabel('Speed, m/s');

subplot(2,2,4)
hold on;
plot(t_vec, V, '-r', 'LineWidth', 2);
plot(t_vec, Vg, '-r', 'LineWidth', 2);
grid on;
grid  minor;
xlabel('Time, s');
ylabel('Speed, m/s');


