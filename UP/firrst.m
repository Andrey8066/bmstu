x = linspace(-2*pi, 2*pi, 500);
y1 = sin(x);
y2 = sin(7*x);

subplot(2, 1, 1);
plot(x, y1, 'LineWidth',4, 'Color','c', 'LineStyle','-.', 'Marker', 's');
hold on;
plot(x, y2);
axis([ -2*pi, 2*pi, -1.2, 1.2]);
grid on; 
title("Графики");
xlabel("x");
ylabel("y");
legend('sin(x)', 'sin(7x)');

subplot(2, 1, 2);
plot(x, y1, 'LineWidth',4, 'Color','c', 'LineStyle','-.', 'Marker', 's');
grid on;
plot(x, y2);
hold on;
axis([ 0*pi, 2*pi, -1.2, 1.2]);
title("Графики");
xlabel("x");
ylabel("y");
legend('sin(x)', 'sin(7x)');
