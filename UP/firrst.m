x = linspace(-2*pi, 2*pi);
y1 = sin(x);
y2 = sin(7*x);

plot(x, y1, 'LineWidth',2, 'Color','c', 'LineStyle','-.', 'Marker', 's');
hold on; 
plot(x, y2);
axis([ -2*pi, 2*pi, -1, 1]);
grid on; 
title("Графики");
xlabel("x");
ylabel("y");
legend('sin(x)', 'sin(7x)');