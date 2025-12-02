function dX = difur(t, X)
global inputStruct;
k = inputStruct.k;
m = inputStruct.m;
g = inputStruct.g;
r0 = inputStruct.r0;

dX = zeros(4,1);

dX(1) = X(3);
dX(2) = X(4);
dX(3) = X(1) * X(4)^2 - k / m * (X(1) - r0) + g*cos(X(2));
dX(4) = -2 * X(3) * X(4) / (X(1)) - g*sin(X(2))/X(1);
end