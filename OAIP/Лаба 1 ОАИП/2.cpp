#include <iostream>
#include <bits/stdc++>

using namespace std;

void print(int n, int a[]) //вывод массива
{
    for ( int i = 0; i < n; i++) cout << a[i] << " ";
    cout << endl;
}
bool check_sorted(int n, int a[]) // проверка массива на отсортированность
{
    for (int i = 0; i<n-1; i++) if (a[i]<a[i+1]) return 0;
    return 1;
}

int* sort(int n, int a[]) // сортировка массива
{
    int res[n];
    return res;
}
int find_min (vector<int> mas)
{
    int res = mas[0];
    for (int i=0, i<mas.size(), i++)
    {
        if (res>mas[i]) res = mas[i];
    }
    return res;
}
int main()
{
    int n = 10;
    int sum = 0;
    int max = 0;
    vector <int> a= {21, 543, 86, 94, 80, 12, 48, 59, 47, 1};
    cout << "Введенный масив:" << endl;
    for ( int i = 0; i < n; i++) // перебор массива для вывода + вычисления суммы + поиска максимального
    {
         cout << a[i] << " ";
         sum += a[i];
         if (a[i] > max) max = a[i];
    }
    cout << endl << "Сумма эллементов массива: "<< sum << endl << "Максимальный эллемент массива: " << max << endl << "Отсортированный массив:" << endl; // вывод всех найденых значений
    print(n, sort(n, res)); // вывод отсортированного массива

    return 0;

}

