function dx = simple_plane_ode(t, x)

global g;

V = sqrt(x(4) ^ 2 + x(5) ^ 2 + x(6) ^ 2);
Vg = sqrt(x(4) ^ 2 + x(6) ^2 );

nxt = 0;
nyt = 0;
nzt = 0;

dx = zeros(1, 6);

dx(1) = x(4);
dx(2) = x(5);
dx(3) = x(6);
dx(4) = g * x(4) / V * ( nxt - x(5) / V * nyt ) - g * x(6) * nzt / Vg;
dx(5) = g * x(5) / V * nxt + g * Vg / V * nyt - g;
dx(6) = g * x(6) / V * (nxt  - x(5) / Vg * nyt ) + g*x(4)*nxt/Vg;

end
