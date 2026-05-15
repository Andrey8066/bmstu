%% lab_parsing.m
% Лабораторная работа №4
% Обработка телеметрической информации GPS Almanac

clc;
clear;
close all;

%% ========================================================================
% 1. Загрузка данных
% ========================================================================

DATA = fileread('\\tsclient\share\Documents\BMSTU\TKT\Лабораторная работа 4\lab_checksum_data.txt');

expr_match = '^[$]PSTMALMANAC,\d+,\d+,\w+[*]\w+';

match = regexp(DATA, expr_match, 'match', 'lineanchors');

%% ========================================================================
% 2. Проверка контрольных сумм
% ========================================================================

disp('========== CHECKSUM TEST ==========');

for i = 1:length(match)

    cmd = match{i};

    expr_tokens = '^[$]PSTMALMANAC,(\d+),(\d+),(\w+)[*](\w+)';

    tokens = regexp(cmd, expr_tokens, 'tokens', 'lineanchors');

    tokens = tokens{1};

    satID       = str2double(tokens{1});
    hexDataSize = str2double(tokens{2});
    hexData     = tokens{3};
    txChecksum  = tokens{4};

    txChecksumDec = uint8(hex2dec(txChecksum));


    calcChecksum = uint8(cmd(1));

    for k = 1:length(cmd)

        if cmd(k) == '*'
            break;
        end

        calcChecksum = bitxor(calcChecksum, uint8(cmd(k)));

    end


    isEqual = (txChecksumDec == calcChecksum);

    disp([ ...
        'SAT = ', num2str(satID), ...
        ' | TX = ', num2str(txChecksumDec), ...
        ' | CALC = ', num2str(calcChecksum), ...
        ' | OK = ', num2str(isEqual) ...
        ]);

end

%% ========================================================================
% 3. Парсинг данных GPS Almanac
% ========================================================================

disp(' ');
disp('========== GPS ALMANAC ==========');

gpsAlmanac = [];

for i = 1:length(match)

    cmd = match{i};

    expr_tokens = ...
        '^[$]PSTMALMANAC,(\d+),(\d+),(\w+)[*](\w+)';

    tokens = regexp(cmd, expr_tokens, ...
        'tokens', 'lineanchors');

    tokens = tokens{1};

    satID       = str2double(tokens{1});
    hexDataSize = str2double(tokens{2});
    hexData     = tokens{3};

    if satID < 1 || satID > 32
        continue;
    end

    bytes = zeros(1, hexDataSize, 'uint8');

    idx = 1;

    for k = 1:2:length(hexData)

        bytes(idx) = uint8(hex2dec(hexData(k:k+1)));
        idx = idx + 1;

    end

    p = 1;

    satid = bytes(p);
    p = p + 1;

    week = typecast(uint8(bytes(p:p+1)), 'uint16');
    week = double(week) - 1023;
    p = p + 2;

    T0a = double(bytes(p)) * 2^12;
    p = p + 1;

    e_raw = typecast(uint8(bytes(p:p+1)), 'uint16');
    e = single(double(e_raw) * 2^-21);
    p = p + 2;

    di_raw = typecast(uint8(bytes(p:p+1)), 'int16');

    di = single(double(di_raw) * 2^-19 * pi);
    p = p + 2;


    Omegadot_raw = typecast(uint8(bytes(p:p+1)), 'int16');

    Omegadot = single(double(Omegadot_raw) * 2^-38 * pi);
    p = p + 2;


    sqrtA_raw = uint32(bytes(p)) + ...
                bitshift(uint32(bytes(p+1)), 8) + ...
                bitshift(uint32(bytes(p+2)), 16);

    sqrtA = single(double(sqrtA_raw) * 2^-11);
    p = p + 3;

    Omega0_raw = uint32(bytes(p)) + ...
                 bitshift(uint32(bytes(p+1)), 8) + ...
                 bitshift(uint32(bytes(p+2)), 16);

    Omega0_raw = bitshift(Omega0_raw, -8);

    Omega0 = single(double(int32(Omega0_raw)) * 2^-23 * pi);
    p = p + 3;

    omega_raw = uint32(bytes(p)) + ...
                bitshift(uint32(bytes(p+1)), 8) + ...
                bitshift(uint32(bytes(p+2)), 16);

    omega_raw = bitshift(omega_raw, -8);

    omega = single(double(int32(omega_raw)) * 2^-23 * pi);
    p = p + 3;

    M0_raw = uint32(bytes(p)) + ...
             bitshift(uint32(bytes(p+1)), 8) + ...
             bitshift(uint32(bytes(p+2)), 16);

    M0_raw = bitshift(M0_raw, -8);

    M0 = single(double(int32(M0_raw)) * 2^-23 * pi);
    p = p + 3;

    af0_raw = typecast(uint8([bytes(p:p+1)]), 'int16');

    af0 = single(double(af0_raw) * 2^-20);
    p = p + 2;

    af1_raw = int16(bytes(p));

    af1_raw = bitshift(af1_raw, 3);
    af1_raw = bitshift(af1_raw, -5);

    af1 = single(double(af1_raw) * 2^-38);
    p = p + 1;

    flags = bytes(p);

    health   = logical(bitand(flags, 1));
    available = logical(bitand(flags, 2));

    alm = struct();

    alm.satid      = satid;
    alm.week       = week;
    alm.T0a        = T0a;

    alm.e          = e;

    alm.delta_i    = di;
    alm.OmegaDot   = Omegadot;

    alm.sqrtA      = sqrtA;

    alm.Omega0     = Omega0;
    alm.omega      = omega;
    alm.M0         = M0;

    alm.af0        = af0;
    alm.af1        = af1;

    alm.health     = health;
    alm.available  = available;

    gpsAlmanac = [gpsAlmanac; alm];

end

satid      = [gpsAlmanac.satid]';
week       = [gpsAlmanac.week]';
T0a        = [gpsAlmanac.T0a]';

e          = [gpsAlmanac.e]';

delta_i    = [gpsAlmanac.delta_i]';
OmegaDot   = [gpsAlmanac.OmegaDot]';

sqrtA      = [gpsAlmanac.sqrtA]';

Omega0     = [gpsAlmanac.Omega0]';
omega      = [gpsAlmanac.omega]';
M0         = [gpsAlmanac.M0]';

af0        = [gpsAlmanac.af0]';
af1        = [gpsAlmanac.af1]';

health     = [gpsAlmanac.health]';
available  = [gpsAlmanac.available]';

T = table( ...
    satid, ...
    week, ...
    T0a, ...
    e, ...
    delta_i, ...
    OmegaDot, ...
    sqrtA, ...
    Omega0, ...
    omega, ...
    M0, ...
    af0, ...
    af1, ...
    health, ...
    available);

disp(T);