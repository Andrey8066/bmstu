function [x] = minx(func, x0, e)
if func(x0 + e) < func(x0 - e)
    x = x0 + e
else
    x = x0 - e
end
end