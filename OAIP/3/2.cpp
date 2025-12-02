#include <iostream>

using namespace std;
int determine(int cords[][3])
{
    return cords[0][0] * (cords[1][1] * cords[2][2] - cords[1][2] * cords[2][1]) - cords[0][1] * (cords[1][0] * cords[2][2] - cords[1][2] * cords[2][0]) + cords[0][2] * (cords[1][0] * cords[2][1] - cords[1][1] * cords[2][0]);
}

void print(int cords[][3])
{
    for (int i = 0; i< 3; i++)
    {
        for (int j = 0; j< 3; j++)
        {
            cout << cords[i][j]<< " ";
        }
        cout << endl;
    }
}
int main()
{
    int cords[3][3];
    for (int i = 0; i< 3; i++)
    {
        cout << "Введите координату x " << i << " точки: ";
        cin >> cords[i][0];
        cout << "Введите координату y " << i << "точки: ";
        cin >> cords[i][1];
    }
    cords [0][2] = 1;
    cords [1][2] = 1;
    cords [2][2] = 1;
    print(cords);
    int det = determine(cords);
    cout << det<< endl;
    if (det == 0 )
    {
        cout << "Коллинеарны" << endl;
    }
    else
    {
        cout << "Неколлинеарны" << endl;
    }
}