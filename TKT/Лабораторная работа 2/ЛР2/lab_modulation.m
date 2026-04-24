
clear; close all; clc;

%% Параметры моделирования
T_stop = 1;              % длительность моделирования (секунд)
fs = 10000;              % частота дискретизации (Гц) = 1/0.0001

%% ==================== 1. АМПЛИТУДНАЯ МОДУЛЯЦИЯ ====================
disp('1. Моделирование AM...');

% Открываем модель (без открытия set_param не сработает)
open('lab_modulation_am.slx');

% Настраиваем параметры модели и её подсистем
set_param('lab_modulation_am', 'Solver', 'FixedStepDiscrete', 'FixedStep', '0.0001');
set_param('lab_modulation_am/m', 'Value', '1');   % коэффициент модуляции m = 1
ws = get_param('lab_modulation_am', 'ModelWorkspace');
ws.assignin('variance', 0.05);

% Запускаем моделирование
out_am = sim('lab_modulation_am', 'StopTime', num2str(T_stop));

% Получаем сигнал из переменной yout (1 - номер порта выхода)
% yout{1}.Values.Data - вектор значений сигнала
% yout{1}.Values.Time - вектор времени
am_signal = out_am.yout{1}.Values.Data;
am_time = out_am.yout{1}.Values.Time;

% Закрываем модель
bdclose('lab_modulation_am');

fprintf('AM моделирование завершено. Длина сигнала: %d отсчётов\n', length(am_signal));

%% ==================== 2. ЧАСТОТНАЯ МОДУЛЯЦИЯ ====================
disp('2. Моделирование FM...');

% Открываем модель
open('lab_modulation_fm.slx');

% Настраиваем параметры
set_param('lab_modulation_fm', 'Solver', 'FixedStepDiscrete', 'FixedStep', '0.0001');
set_param('lab_modulation_fm/dW', 'Value', '50*2*pi');   % девиация частоты 50 Гц
ws = get_param('lab_modulation_fm', 'ModelWorkspace');
ws.assignin('variance', 0.05);
ws.assignin('frequencyDeviation', 50);

% Запускаем моделирование
out_fm = sim('lab_modulation_fm', 'StopTime', num2str(T_stop));

% Получаем сигнал
fm_signal = out_fm.yout{1}.Values.Data;
fm_time = out_fm.yout{1}.Values.Time;

% Закрываем модель
bdclose('lab_modulation_fm');

fprintf('FM моделирование завершено. Длина сигнала: %d отсчётов\n', length(fm_signal));

%% ==================== 3. ФАЗОВАЯ МОДУЛЯЦИЯ ====================
disp('3. Моделирование PM...');

% Открываем модель
open('lab_modulation_pm.slx');

% Настраиваем параметры
set_param('lab_modulation_pm', 'Solver', 'FixedStepDiscrete', 'FixedStep', '0.0001');
set_param('lab_modulation_pm/dPhi', 'Value', 'pi/4');
ws = get_param('lab_modulation_pm', 'ModelWorkspace');
ws.assignin('variance', 0.05);
ws.assignin('phaseDeviation', pi/4);

% Запускаем моделирование
out_pm = sim('lab_modulation_pm', 'StopTime', num2str(T_stop));

% Получаем сигнал
pm_signal = out_pm.yout{1}.Values.Data;
pm_time = out_pm.yout{1}.Values.Time;

% Закрываем модель
bdclose('lab_modulation_pm');

fprintf('PM моделирование завершено. Длина сигнала: %d отсчётов\n', length(pm_signal));

%% ==================== 4. ПОСТРОЕНИЕ СПЕКТРОВ ====================
disp('4. Построение амплитудных спектров...');

% Проверка, что сигналы не пустые
if isempty(am_signal) || isempty(fm_signal) || isempty(pm_signal)
    error('Один из сигналов пуст. Проверьте, что в моделях есть блок Out1');
end

% Создаём окно для графиков
figure('Name', 'Спектры модулированных сигналов', 'NumberTitle', 'off', 'Position', [100 100 1200 500]);

% Спектр AM
plot_spectrum(am_signal, fs);
hold on;
% Спектр FM
plot_spectrum(fm_signal, fs);
hold on;

% Спектр PM
plot_spectrum(pm_signal, fs);
xlim([0 200]);
xlabel('Частота, Гц');
ylabel('Амплитуда');
grid on;
legend( 'AM (m = 1)', 'FM (\Deltaf = 50 Гц)', 'PM (\Delta\phi = \pi/4)');

%% ==================== 5. ВСПОМОГАТЕЛЬНАЯ ФУНКЦИЯ ====================
function plot_spectrum(signal, fs)
    % Функция построения одностороннего амплитудного спектра
    % Вход:
    %   signal - входной сигнал (вектор-столбец)
    %   fs - частота дискретизации (Гц)
    %   title_str - заголовок графика
    
    % 1. Удаляем постоянную составляющую (как в лекции 2)
    signal = signal - mean(signal);
    
    % 2. Длина сигнала
    N = length(signal);
    
    % 3. Применяем оконную функцию Ханна (убирает спектральную утечку)
    window = hann(N);
    signal_windowed = signal .* window;
    
    % 4. Вычисляем БПФ
    S = fft(signal_windowed, N);
    
    % 5. Односторонний амплитудный спектр (нормировка на N)
    half_N = floor(N/2);                       % половина спектра
    S_one_sided = abs(S(1:half_N)) / N;        % нормировка
    S_one_sided(2:end) = S_one_sided(2:end) * 2; % компенсация энергии
    
    % 6. Частотная ось (от 0 до fs/2)
    f = (0:half_N-1) * fs / N;
    
    % 7. Построение графика
    plot(f, S_one_sided, 'LineWidth', 1.5);
    
end