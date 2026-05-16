clc;
clear;
close all;

x = 46;

%% RS-423

bits = de2bi(x, 8, 'right-msb');

bits = [0, bits, 1, 1];

signal = [];

for i = 1:length(bits)
    if bits(i) == 1
        signal = [signal, -6];
    else
        signal = [signal, 6];
    end
end

t = 0:length(signal)-1;

figure;

stairs(t, signal, 'LineWidth', 2);

grid on;

ylim([-12 12]);
xlim([0 length(signal)]);

title("RS-423 (Асинхронный, LSB-FIRST)");

xlabel('Время');
ylabel('Уровень сигнала');

for i = 1:length(signal)

    x_text = i - 0.5;

    text(x_text, 8, num2str(bits(i)), ...
        'HorizontalAlignment', 'center', ...
        'FontSize', 12, ...
        'FontWeight', 'bold');
end

text(0.5, -9, 'START');
text(length(signal)/2, -9, 'DATA');
text(length(signal)-1.5, -9, 'STOP');

saveas(gcf, '\\tsclient\share\Documents\BMSTU\TKT\Домашняя работа 2\images/rs423.png');
%% RS-485

signal1 = signal;
signal2 = -signal;

t = 0:length(signal)-1;

figure;

subplot(2,1,1);

stairs(t, signal1, 'LineWidth', 2);

grid on;

ylim([-12 12]);
xlim([0 length(signal)]);

title("RS-485 Линия A");

xlabel('Время');
ylabel('Уровень');

for i = 1:length(signal)

    x_text = i - 0.5;

    text(x_text, 8, num2str(bits(i)), ...
        'HorizontalAlignment', 'center', ...
        'FontWeight', 'bold');
end

text(0.5, -9, 'START');
text(length(signal)/2, -9, 'DATA');
text(length(signal)-1.5, -9, 'STOP');

subplot(2,1,2);

stairs(t, signal2, 'LineWidth', 2);

grid on;

ylim([-12 12]);
xlim([0 length(signal)]);

title("RS-485 Линия B");

xlabel('Время');
ylabel('Уровень');

for i = 1:length(signal)

    x_text = i - 0.5;

    text(x_text, 8, num2str(bits(i)), ...
        'HorizontalAlignment', 'center', ...
        'FontWeight', 'bold');
end

text(0.5, -9, 'START');
text(length(signal)/2, -9, 'DATA');
text(length(signal)-1.5, -9, 'STOP');
saveas(gcf, '\\tsclient\share\Documents\BMSTU\TKT\Домашняя работа 2\images/rs485.png');

%% SPI

bits = de2bi(x, 8, 'left-msb');

N = length(bits);

clk = [];
mosi = [];

for i = 1:N

    clk  = [clk 0 1];
    mosi = [mosi bits(i) bits(i)];
end

clk  = [clk clk(end)];
mosi = [mosi mosi(end)];

cs = [1 zeros(1, length(clk)-3) 1];
cs = [cs cs(end)];

t = 0:length(clk)-1;

figure;

subplot(3,1,1);

stairs(t, cs, 'LineWidth', 2);

ylim([-0.5 1.5]);
xlim([0 length(clk)-1]);

grid on;

ylabel('CS');

text(length(clk)/2, -0.3, 'CS ACTIVE');

subplot(3,1,2);

stairs(t, clk, 'LineWidth', 2);

ylim([-0.5 1.5]);
xlim([0 length(clk)-1]);

grid on;

ylabel('CLK');

text(length(clk)/2, -0.3, 'CLOCK');

subplot(3,1,3);

stairs(t, mosi, 'LineWidth', 2);

ylim([-0.5 1.5]);
xlim([0 length(clk)-1]);

grid on;

ylabel('MOSI');
xlabel('Время');

text(length(clk)/2, -0.3, 'DATA');

hold on;

for i = 1:N

    text(2*i-1, 1.2, num2str(bits(i)), ...
        'HorizontalAlignment', 'center', ...
        'FontWeight', 'bold');
end
saveas(gcf, '\\tsclient\share\Documents\BMSTU\TKT\Домашняя работа 2\images/spi.png');

%% I2C

addr = hex2dec('14');

addr_bits = de2bi(addr, 7, 'left-msb');

rw = 0;

data = 46;

data_bits = de2bi(data, 8, 'left-msb');

ack = 0;

bits = [
    addr_bits rw ack ...
    data_bits ack
];

N = length(bits);

scl = [];
sda = [];

sda = [1 0];
scl = [1 1];

for i = 1:N

    bit = bits(i);

    sda = [sda bit bit];

    scl = [scl 0 1];
end

sda = [sda 0 1];
scl = [scl 1 1];

sda = [sda sda(end)];
scl = [scl scl(end)];

t = 0:length(scl)-1;

figure;

subplot(2,1,1);

stairs(t, scl, 'LineWidth', 2);

ylim([-0.5 1.5]);
xlim([0 length(t)-1]);

grid on;

ylabel('SCL');

text(length(t)/2, -0.3, 'CLOCK');

subplot(2,1,2);

stairs(t, sda, 'LineWidth', 2);

ylim([-0.5 1.5]);
xlim([0 length(t)-1]);

grid on;

ylabel('SDA');
xlabel('Время');

hold on;

labels = [string(addr_bits) "W" "ACK" string(data_bits) "ACK"];

for i = 1:length(labels)

    x_text = 3 + 2*(i-1);

    text(x_text, 1.2, labels(i), ...
        'HorizontalAlignment', 'center', ...
        'FontWeight', 'bold');
end

text(1, -0.3, 'START');
text(10, -0.3, 'ADDRESS');
text(22, -0.3, 'DATA');
text(length(t)-3, -0.3, 'STOP');
saveas(gcf, '\\tsclient\share\Documents\BMSTU\TKT\Домашняя работа 2\images/i2c.png');

%% USB 2.0

bits = de2bi(x, 8);

stuffed = [];

ones_count = 0;

for i = 1:length(bits)

    stuffed = [stuffed bits(i)];

    if bits(i) == 1
        ones_count = ones_count + 1;
    else
        ones_count = 0;
    end

    if ones_count == 6

        stuffed = [stuffed 0];

        ones_count = 0;
    end
end

bits = stuffed;

level = 1;

dp = [];
dm = [];

for i = 1:length(bits)

    bit = bits(i);

    if bit == 0
        level = ~level;
    end

    dp = [dp level level];
    dm = [dm ~level ~level];
end

dp = [dp dp(end)];
dm = [dm dm(end)];

t = 0:length(dp)-1;

figure;

subplot(2,1,1);

stairs(t, dp, 'LineWidth', 2);

ylim([-0.5 1.5]);
xlim([0 length(t)-1]);

grid on;

ylabel('D+');

title('USB 2.0 NRZI');

text(length(t)/2, -0.3, 'NRZI DATA');

subplot(2,1,2);

stairs(t, dm, 'LineWidth', 2);

ylim([-0.5 1.5]);
xlim([0 length(t)-1]);

grid on;

ylabel('D-');
xlabel('Время');

text(length(t)/2, -0.3, 'INVERTED');

hold on;

for i = 1:length(bits)

    x_text = 2*i - 1;

    text(x_text, 1.2, num2str(bits(i)), ...
        'HorizontalAlignment', 'center', ...
        'FontWeight', 'bold');
end
saveas(gcf, '\\tsclient\share\Documents\BMSTU\TKT\Домашняя работа 2\images/usb20.png');

%% 1-Wire

addr = hex2dec('14');

addr_bits = de2bi(addr, 8);

data = 46;

data_bits = de2bi(data, 8);

bits = [addr_bits data_bits];

dq = [];
labels_x = [];

for i = 1:length(bits)

    bit = bits(i);

    if bit == 1

        dq = [dq 0 1 1 1];

    else

        dq = [dq 0 0 0 1];
    end

    labels_x = [labels_x length(dq)-2];
end

dq = [dq dq(end)];

t = 0:length(dq)-1;

figure;

stairs(t, dq, 'LineWidth', 2);

ylim([-0.5 1.5]);
xlim([0 length(t)-1]);

grid on;

xlabel('Время');
ylabel('DQ');

title('1-Wire');

hold on;

for i = 1:length(bits)

    text(labels_x(i), 1.2, num2str(bits(i)), ...
        'HorizontalAlignment', 'center', ...
        'FontWeight', 'bold');
end

text(14, -0.3, 'ADDRESS');
text(46, -0.3, 'DATA');
saveas(gcf, '\\tsclient\share\Documents\BMSTU\TKT\Домашняя работа 2\images/1wire.png');