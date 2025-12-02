#include <iostream>
#include <string>

using namespace std;

void sort(string *name, string *author, int *year, int *copy,
          int n) { // ФУнкция для сортировки данных по названию книги
  string *name_ptr = name; // Указатели
  string *author_ptr = author;
  int *year_ptr = year;
  int *copy_ptr = copy;

  string name_replace; // Переменные используемые для перестановки при
                       // сортировке
  string author_replace;
  int year_replace;
  int copy_replace;

  for (int i = 0; i < n - 1; i++) {
    for (int j = 0; j < n - 1; j++) {
      int x = 1;
      while (x < sizeof(*(name_ptr + j)) && x < sizeof(*(name_ptr + j + 1))) {
        if (static_cast<int>((*(name_ptr + j))[x]) >
            static_cast<int>((*(name_ptr + j + 1))[x])) // Сравнение первых букв
        {
          name_replace = *(name_ptr + j); // замена названий
          *(name_ptr + j) = *(name_ptr + j + 1);
          *(name_ptr + j + 1) = name_replace;

          author_replace = *(author_ptr + j); // замена авторов
          *(author_ptr + j) = *(author_ptr + 1 + j);
          *(author_ptr + 1 + j) = author_replace;

          year_replace = *(year_ptr + j); // замена авторов
          *(year_ptr + j) = *(year_ptr + 1 + j);
          *(year_ptr + 1 + j) = year_replace;

          copy_replace = *(copy_ptr + j); // замена авторов
          *(copy_ptr + j) = *(copy_ptr + 1 + j);
          *(copy_ptr + 1 + j) = copy_replace;
          break;
        }
        else if (static_cast<int>((*(name_ptr + j))[x]) ==
            static_cast<int>((*(name_ptr + j + 1))[x]))
            {
              x+=1;
            }
        else break;
      }
    }
  }
}

int main() {
  string *name, *author; // Массивы с названием книги и именем автора
  int *year, *copy; // Массивы с годом издания и количеством копий
  int n;
  cout
      << "Введите колличество книг в библиотеке: "; // Ввод количества книг -
                                                    // количества итераций цикла
  cin >> n;

  name = new string[n]; // указатели
  author = new string[n];
  year = new int[n];
  copy = new int[n];

  string *nptr = name;
  string *aptr = author;
  int *yptr = year;
  int *cptr = copy;

  char thrash;

  for (int i = 0; i < n; i++) // Ввод данных
  {
    cout << endl << "Введите название книги " << i + 1 << ": ";
    cin >> thrash;
    getline(cin, *nptr);
    *nptr = thrash + (*nptr);

    cout << endl << "Введите ФИО автора  книги " << i + 1 << ": ";
    cin >> thrash;
    getline(cin, *aptr);
    *aptr = thrash + *aptr;

    cout << endl << "Введите дату выпуска книги " << i + 1 << ": ";
    cin >> *yptr;

    cout << endl << "Введите количество копий книги " << i + 1 << ": ";
    cin >> *cptr;

    if (i < n - 1) { // Ограничения чтобы не выйти за предел массива
      nptr++;
      aptr++;
      yptr++;
      cptr++;
    }
  }
  sort(name, author, year, copy, n); // сортировка данных

  nptr -= n - 1; // сброс указателей
  aptr -= n - 1;
  yptr -= n - 1;
  cptr -= n - 1;

  for (int j = 0; j < n; j++) // вывод отсортированных данных
  {
    cout << j + 1 << ". Название книги: " << *(nptr) << ". Автор: " << *(aptr)
         << ". Год издания: " << *(yptr) << ". Количество копий: " << *(cptr)
         << endl;

    if (j < n - 1) { // Ограничения чтобы не выйти за предел массива
      nptr++;
      aptr++;
      yptr++;
      cptr++;
    }
  }
}