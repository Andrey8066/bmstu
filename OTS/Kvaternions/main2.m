A = [sqrt(3)/2;sqrt(3)/6;-sqrt(3)/6;-sqrt(3)/6];

conjA = myConjugate(A);

ex = [0; 1; 0; 0];

rx = myMultiply(myMultiply(A, ex), conjA);

ey = [0; 0; 1; 0];

ry = myMultiply(myMultiply(A, ey), conjA);

ez = [0; 0; 0; 1];

rz = myMultiply(myMultiply(A, ez), conjA);

ROT = [rx ry rz];

disp(ROT);

ROT = ROT(2:4, :);
e1i = [1;1;1];
r1i = [0;1;1;1];

e1e = ROT' * e1i  
r1e = myMultiply(myMultiply(conjA, r1i), A)


e2e = [1;1;1];
r2e = [0;1;1;1];

e2i = ROT * e2e 
r2i = myMultiply(myMultiply(A, r2e), conjA)