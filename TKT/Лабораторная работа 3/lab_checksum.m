clc;
clear;
close all;

poly = [1 0 0 1 1];

crcGen = comm.CRCGenerator( ...
    'Polynomial', poly, ...
    'ChecksumsPerFrame', 1);

crcDet = comm.CRCDetector( ...
    'Polynomial', poly, ...
    'ChecksumsPerFrame', 1);

msg_dec = 46;

msg = de2bi(msg_dec, 8, 'left-msb')';

encoded = step(crcGen, logical(msg));

msg_len = length(msg);
enc_len = length(encoded);

redundancy = enc_len - msg_len;

crc_bits = encoded(end-3:end);

err_pos = length(encoded) - 5 + 1;

error_dec = 2^(5-1);

error_vec = de2bi(error_dec, length(encoded), 'left-msb')';

received = bitxor(encoded, logical(error_vec));

[decoded, errFlag] = step(crcDet, received);

disp('==============================');
disp('Кодовый полином:');
disp(num2str(poly));

disp('==============================');
disp(['Исходное сообщение (dec): ', num2str(msg_dec)]);
disp('Исходное сообщение (bin):');
disp(num2str(msg'));

disp('==============================');
disp('CRC остаток:');
disp(num2str(crc_bits'));

disp('==============================');
disp('Закодированное сообщение:');
disp(num2str(encoded'));

disp('==============================');
disp('Модель ошибки:');
disp(num2str(error_vec));

disp('==============================');
disp('Полученное сообщение:');
disp(num2str(received'));

disp('==============================');
disp('Декодированное сообщение:');
disp(num2str(decoded'));

disp('==============================');
disp(['Ошибка обнаружена: ', num2str(errFlag)]);

[~, syndrome] = step(crcDet, received);

disp('==============================');
disp(['Синдром ошибки: ', num2str(syndrome)]);

n = length(encoded);

syndrome_table = zeros(n, 1);

disp('==============================');
disp('Таблица синдромов');

syndrome_table = zeros(n, 4);

for i = 1:n
    err = zeros(n,1);
    err(i) = 1;
    test_word = bitxor(encoded, logical(err));
    temp = step(crcGen, test_word(1:end-4));
    syndrome = temp(end-3:end);
    syndrome_table(i,:) = syndrome';

end

bit_number = n - err_pos + 1;

T = table((1:n)', syndrome_table, ...
    'VariableNames', {'BitNumber', 'Syndrome'});

disp(T);

disp(['Ошибочный бит: ', num2str(bit_number)]);

data7 = de2bi(46, 7, 'left-msb');

parity = data7(1);

for i = 2:length(data7)
    parity = bitxor(parity, data7(i));
end

xor_encoded = [parity data7];

disp('==============================');
disp('XOR-кодирование');
disp(num2str(xor_encoded));

err1 = de2bi(2^(5-1), 8, 'left-msb');

recv1 = bitxor(xor_encoded, err1);

parity_check1 = mod(sum(recv1), 2);

disp('==============================');
disp('Ошибка в одном бите');

disp('Полученное сообщение:');
disp(num2str(recv1));

if parity_check1 == 0
    disp('Ошибка НЕ обнаружена');
else
    disp('Ошибка обнаружена');
end

err2_dec = 2^(5-1) + 2^(3-1);

err2 = de2bi(err2_dec, 8, 'left-msb');

recv2 = bitxor(xor_encoded, err2);

parity_check2 = mod(sum(recv2), 2);

disp('==============================');
disp('Ошибка в двух битах');
disp('Полученное сообщение:');
disp(num2str(recv2));

if parity_check2 == 0
    disp('Ошибка НЕ обнаружена');
else
    disp('Ошибка обнаружена');
end

