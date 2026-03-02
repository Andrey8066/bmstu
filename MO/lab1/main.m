[x, y, k] = bruteforce(@optimisationFunction, -5, 5, 0.001)
[x, y, k] = dihotom(@optimisationFunction, -5, 5, 0.001)
[x, y, k] = gold(@optimisationFunction, -5, 5, 0.001)
