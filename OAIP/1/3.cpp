#include <iostream>

using namespace std;

int main()
{
    int a, b, c, m;
    char z;
    cout<<"Введите 1 число: \n";
    cin>> a;

    cout<<"Введите знак: \n";
    cin>> z;
    switch(z)
    {
        case '+':
            cout<<"Введите слагаемое: \n";
            cin>> m;
            b = m+a;
            break;
        case '*':
            cout<<"Введите множитель: \n";
            cin>> m;
            b = m*a;
            break;
        case '/':
            cout<<"Введите делитель: \n";
            cin>> m;
            b = a/m;
            break;
        case '-':
            cout<<"Введите частно: \n";
            cin>> m;
            b = a-m;
            break;
    }
    
    c = b+a;
    cout<<"A: "<<a<<"\n";
    cout<<"B: "<<b<<"\n";
    cout<<"C: "<<c<<"\n";
    return 0;
}

