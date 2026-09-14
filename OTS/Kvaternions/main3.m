a = [2;-2;1];
a = a/norm(a)
A = [sqrt(3)/2; a(1)*sin(pi/6); a(2)*sin(pi/6);a(3)*sin(pi/6)]

b = [1;1;1];
b = b/norm(b)
M = [cosd(15); b(1)*sind(15); b(2)*sind(15);b(3)*sind(15)]

N = myMultiply(M, A)

c1 = [1; -2; -2];
rc1i = [0;c1]

rc1e = myMultiply(myMultiply(myConjugate(N), rc1i), N)
