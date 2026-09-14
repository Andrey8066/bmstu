function P = myConjugate(A)
    P = zeros(4, 1);
    P(1) = A(1);
    P(2) = -A(2);
    P(3) = -A(3);
    P(4) = -A(4);
end

