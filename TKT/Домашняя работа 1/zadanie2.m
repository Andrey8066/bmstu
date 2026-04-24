clc; clear; close all;

%% ---------- Путь сохранения ----------
scriptPath = fileparts(mfilename('fullpath'));
if isempty(scriptPath)
    scriptPath = pwd;
end

outDir = fullfile(scriptPath, 'results');
if ~exist(outDir, 'dir')
    mkdir(outDir);
end

%% ---------- Исходные данные ----------
data = [0 0 1 0 1 1 1 0]; % 46 = 00101110 (MSB → LSB)

Tb = 1;              
samples = 100;       

t = 0:Tb/samples:Tb*length(data)-Tb/samples;

expand = @(bits) repelem(bits, samples);

%% ---------- NRZ (однополярный) ----------
nrz_uni = expand(data);

figure; stairs(t, nrz_uni, 'LineWidth', 1.5);
title('NRZ Unipolar'); grid on;
ylim([-0.5 1.5])
exportgraphics(gcf, fullfile(outDir,'NRZ_unipolar.png'),'Resolution',300);

%% ---------- NRZ (биполярный) ----------
nrz_bi = expand(2*data - 1);

figure; stairs(t, nrz_bi, 'LineWidth', 1.5);
title('NRZ Bipolar'); grid on;
ylim([-1.5 1.5])
exportgraphics(gcf, fullfile(outDir,'NRZ_bipolar.png'),'Resolution',300);

%% ---------- NRZ-I (инверсия при 1) ----------
level = -1;
nrz_i1 = zeros(1, length(data));
for i = 1:length(data)
    if data(i) == 1
        level = -level;
    end
    nrz_i1(i) = level;
end
nrz_i1 = expand(nrz_i1);

figure; stairs(t, nrz_i1, 'LineWidth', 1.5);
title('NRZ-I (инверсия при 1)'); grid on;
ylim([-1.5 1.5])
exportgraphics(gcf, fullfile(outDir,'NRZ_I_1.png'),'Resolution',300);

%% ---------- NRZ-I (инверсия при 0) ----------
level = -1;
nrz_i0 = zeros(1, length(data));
for i = 1:length(data)
    if data(i) == 0
        level = -level;
    end
    nrz_i0(i) = level;
end
nrz_i0 = expand(nrz_i0);

figure; stairs(t, nrz_i0, 'LineWidth', 1.5);
title('NRZ-I (инверсия при 0)'); grid on;
ylim([-1.5 1.5])
exportgraphics(gcf, fullfile(outDir,'NRZ_I_0.png'),'Resolution',300);

%% ---------- AMI ----------
ami = zeros(1, length(data));
level = 1;
for i = 1:length(data)
    if data(i) == 1
        ami(i) = level;
        level = -level;
    else
        ami(i) = 0;
    end
end
ami = expand(ami);

figure; stairs(t, ami, 'LineWidth', 1.5);
title('AMI'); grid on;
ylim([-1.5 1.5])
exportgraphics(gcf, fullfile(outDir,'AMI.png'),'Resolution',300);

%% ---------- RZ ----------
rz = [];
for i = 1:length(data)
    if data(i) == 1
        rz = [rz ones(1, samples/2), zeros(1, samples/2)];
    else
        rz = [rz -ones(1, samples/2), zeros(1, samples/2)];
    end
end

figure; stairs(t, rz, 'LineWidth', 1.5);
title('RZ'); grid on;
ylim([-1.5 1.5])
exportgraphics(gcf, fullfile(outDir,'RZ.png'),'Resolution',300);

%% ---------- Manchester ----------
man = [];
for i = 1:length(data)
    if data(i) == 0
        man = [man ones(1, samples/2), -ones(1, samples/2)];
    else
        man = [man -ones(1, samples/2), ones(1, samples/2)];
    end
end

figure; stairs(t, man, 'LineWidth', 1.5);
title('Manchester (IEEE 802.3)'); grid on;
ylim([-1.5 1.5])
exportgraphics(gcf, fullfile(outDir,'Manchester.png'),'Resolution',300);

%% ---------- Differential Manchester ----------
diff_man = [];
level = -1;
for i = 1:length(data)
    if data(i) == 0
        level = -level;
    end
    first = level;
    second = -level;
    diff_man = [diff_man ...
        first*ones(1, samples/2), ...
        second*ones(1, samples/2)];
    level = second;
end

figure; stairs(t, diff_man, 'LineWidth', 1.5);
title('Differential Manchester'); grid on;
ylim([-1.5 1.5])
exportgraphics(gcf, fullfile(outDir,'Diff_Manchester.png'),'Resolution',300);

%% ---------- 2B1Q ----------
pairs = reshape(data, 2, [])';
map = containers.Map({'00','01','10','11'}, [-2.5 -0.833 0.833 2.5]);

levels = zeros(1, size(pairs,1));
for i = 1:size(pairs,1)
    key = num2str(pairs(i,:),'%d%d');
    levels(i) = map(key);
end

b2q = repelem(levels, samples);
t2 = 0:Tb/samples:Tb*length(levels)-Tb/samples;

figure;
stairs(t2, b2q, 'LineWidth', 1.5);
title('2B1Q'); grid on;
ylim([-3.5 3.5])
exportgraphics(gcf, fullfile(outDir,'2B1Q.png'),'Resolution',300);