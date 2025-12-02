pkg load symbolic

syms I11 I22 I33 Uj
% Здесь пишем комплексные величины в ветвях, все значения округляем до целого как в школе
Z1 = sym('10');
Z2 = sym('60 - 30i');
Z3 = sym('60 - 80i');
Z4 = sym('0');
Z5 = sym('60 + 20i');
Z6 = sym('90i');
E3 = sym('10 + 150i');
E4 = sym('-800i');
E5 = sym('-280 + 550i');
J2 = sym('-1 + 2i');

% Здесь пишем уравнения для контурных токов, если получились не целые числа, значит уранения не верны,
% ток в контуре проходящем через источник тока равен току источника
eq1 = I11 == J2 ;
eq2 = I11 * (Z1 + Z5) + I22 * (Z1 + Z5) - Z1 * I33 == E5 + E4;
eq3 = I33 * (Z1 + Z3 + Z6) - I11 * ( Z1 + Z6 ) - I22 * Z1 == E3

% Solve the system
S = solve([eq1, eq2, eq3], [I11, I22, I33]);

% Optional: convert to numeric form
double([S.I11, S.I22, S.I33])

I1 = S.I11 + S.I22 - S.I33
I2 = S.I11
I3 = S.I33
I4 = S.I22
I5 = S.I11 + S.I22
I6 = S.I11 - S.I33

eq1 = Uj + E5  == I2 * Z2 + I6 * Z6  + I1 * Z1 + I5 * Z5;
eq2 = Uj - E4 == I2 * Z2 + I6 * Z6;
eq3 = Uj + E3 + E5 == I2 * Z2 + I3 * Z3 + I5 * Z5;
S = solve([eq1], Uj)
S = solve([eq2], Uj)
S = solve([eq3], Uj)

syms I44 I55 U Z I66

eq4 = I55*(Z1 + Z5)+(I44)*(Z5)==E5 + E4;
%eq4 = I44 * (Z2 + Z3 + Z5) + I55 * (Z5) == E3 + E5;
eq5 = I44 == J2;
eq6 = U + I44 * Z2 == -E4;
eq7 = Z == Z3 + (Z1 * Z5)/(Z1 + Z5);
eq8 = I66 == U / (Z + Z6);

S = solve([eq4, eq5, eq6, eq7, eq8], [I44, I55, U, Z, I66]);
S.I44
S.I55
S.U
S.Z + Z6
S.I66