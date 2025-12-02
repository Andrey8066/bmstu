pkg load symbolic

syms I11 I22 I33 Ubc Z0 I0

% Define given impedances and voltages
Z1 = 40 + 50i;
Z2 = 40 + 60i;
Z3 = -20i;
Z4 = -50i;
Z5 = 0;
Z6 = 50 - 50i;
E2 = -40 - 250i;
E5 = 400 - 600i;
E6 = -880 + 620i;

% Define equations
eq1 = I11 == -4;
eq2 = I22*(Z2 + Z3 + Z4) + I11*(Z2 + Z3) - I33*(Z3) == E2;
eq3 = I33*(Z3 + Z6) - I22*(Z3) - I11*(Z3) == (E5 + E6);

% Solve the system
S = solve([eq1, eq2, eq3], [I11, I22, I33]);

% Optional: convert to numeric form
double([S.I11, S.I22, S.I33])

I1 = S.I11
I2 = S.I11 + S.I22
I3 = S.I11 + S.I22 - S.I33
I4 = S.I22
I5 = S.I11 - S.I33
I6 = S.I33
Uj = (I1*Z1 + I2*Z2 + I3*Z3) + ( - E2 + E5)


%eq4 = (E6 + E5 - I22*(Z6))/(Z6 + Z3) == I11;
eq4 = (-280 - 180i)/(50-70i)== I11;
eq5 = -4 == I22;
eq6 = Ubc + I22*Z2 - I11*Z3 == E2;
eq7 = Z2 + (Z3*Z6)/(Z3+Z6) == Z0;
eq8 = I0 == Ubc/(Z0 + Z4);

disp(eq4);

S = solve([eq4, eq5, eq6, eq7, eq8], [I11, I22, Ubc, Z0, I0]);

disp(S.Ubc)
disp(S.I0)

