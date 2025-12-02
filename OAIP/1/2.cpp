#include <iostream>

using namespace std;

int main()
{
    int a, c, sum;
    int ca;
    string name;
    sum = 0;
    cout<<"Введите количество разных товаров: \n";
    cin>> c;
    for (int i=0; i<c; i++)
    {
        cout<<"Введите название товара: \n";
        cin>> name;
        cout<<"Введите цену "<<name << " : \n";
        cin>> a;
        cout<<"Введите количество "<<name << " : \n";
        cin>> ca;
        cout<<"Цена всех покупок "<<name << " : \n"<<a*ca <<"\n";
        sum+=a*ca;
    }
    cout << "Общая сумма покупок : " << sum << endl;
    return 0;
}

