#include <iostream>

using namespace std;

void print(int n, int a[]) // вывод массива
{
  for (int i = 0; i < n; i++)
    cout << a[i] << " ";
  cout << endl;
}
bool check_sorted(int n, int a[]) // проверка массива на отсортированность
{
  for (int i = 0; i < n - 1; i++)
    if (a[i] < a[i + 1])
      return 0;
  return 1;
}

int *sort(int n, int a[]) // сортировка массива
{
  while (!check_sorted(n, a)) // проверка отсортирован ли массив
  {
    for (int j = n - 1; j > 0; j--) // перебор всех эллементов массива
    {
      if (a[j] > a[j - 1]) // проверка какой из эллементов массива больше
      {
        int s = a[j];
        a[j] = a[j - 1];
        a[j - 1] = s; // замена эллементов массива
        break;
      }
    }
  }
  return a;
}

int main() {
  int n = 10;
  int sum = 0;
  int max = 0;
  int a[n];
  for (int i = 0; i < n; i++) // ввод массива
  {
    cout << "Введите " << i + 1 << " число: ";
    cin >> a[i];
  }
  cout << "Введенный масив:" << endl;
  for (int i = 0; i < n; i++) // перебор массива для вывода + вычисления суммы +
                              // поиска максимального
  {
    cout << a[i] << " ";
    sum += a[i];
    if (a[i] > max)
      max = a[i];
  }
  cout << endl
       << "Сумма эллементов массива: " << sum << endl
       << "Максимальный эллемент массива: " << max << endl
       << "Отсортированный массив:" << endl; // вывод всех найденых значений
  print(n, sort(n, a)); // вывод отсортированного массива

  return 0;
}
