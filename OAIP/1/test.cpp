#include <iostream>

using namespace std;

int factorial(int n)
{
    if (n>1)
    {
    return n*factorial(n-1);
    }
    else
    {
        return n;
    }
}
int main()
{
    int a, b;
    cout<<"Введите 1 число: \n";
    cin>> a;
    cout<<"Факториал равен: "<<factorial(a)<<"\n";
    return 0;
}

