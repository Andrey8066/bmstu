function d = derevative(func, x, i, epsilon)
    ep = eye(length(x))*epsilon;
    ep = ep(i);
    d = (func(x+ep) - func(x))/ep;
endfunction