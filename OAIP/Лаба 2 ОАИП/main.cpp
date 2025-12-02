// Лабораторная ОАИП 2 Вариант 2

#include <iostream>

using namespace std;

long int min(long int a, long int b) { // Поиск минимального числа
  if (a <= b)
    return a;
  return b;
}

int count_a(char *input) { // Подсчет количества слов заканчивающихся на a
  int res = 0;
  char *ptr = input;
  ptr++;
  while (*ptr != input[-1]) {
    if (*ptr == 'a' &&
        (*(ptr + 1) == ' ' or
         *(ptr + 1) ==
             '\0')) { // Условие для подходящих слов ( последняя буква a)
      res++;
    }
    ptr++;
  }
  return res;
}

int find_shortest(char *input) { // Поиск кратчайшего слова
  int res = 50;
  char *ptr = input;
  char *last = input;
  while (*ptr != '\0') {
    if (*ptr == ' ') {
      res = min(ptr - last, res);
      last = ptr + 1;
    }
    ptr++;
  }
  return res;
}

int count_b(char *input) { // Подсчет букв b во втором слове
  int res = 0;
  char *ptr = input;
  while (*ptr != ' ') { // Нахождение первого пробела
    ptr++;
  }
  ptr++; // Начало 2 слова

  while (*ptr != ' ') { // Проверка всех букв слова
    if (*ptr == 'b') {
      res++;
    }
    ptr++;
  }
  return res;
}

void lower(char *input) { // Перевод всех букв в нижний регистр
  char *ptr = input;

  while (*ptr != '\0') {
    if (65 <= static_cast<int>(*ptr) &&
        static_cast<int>(*ptr) <=
            90) // Перевод символов верхнего регистра в нижний
      *ptr = static_cast<char>(*ptr + 32);
    cout << *ptr; // Вывод символов
    ptr++;
  }
  cout << endl;
}

int count_pal(
    char *input) { // Подсчет слов у которых первая и последняя буква совпадают
  char *ptr = input;
  char *last = input;
  int res = 0;

  while (*(ptr + 1) != '\0') {

    while (*ptr != ' ' && *ptr != '\0') { // поиск конца слова
      ptr++;
    }
    if (*(ptr - 1) == *last) // Проверка на совпадение
      res++;
    last = ptr + 1; // Начало следующего слова
    ptr += 1;
    if (*ptr == '\0')
      return res;
  }
  return res;
}

int find_str(char *input) { // Поиск подстроки
  char *ptr = input;
  char *ptr_1 = input;
  while (*(ptr + 3) != '\0') {
    ptr_1 = ptr + 1;
    while (*(ptr_1 + 2) != '\0') {
      if (*ptr == *ptr_1 &&
          *(ptr + 1) == *(ptr_1 + 1)) { // Проверка совпадения подстрок
        cout << *ptr << *(ptr + 1) << endl; // Вывод найденой подстроки
        return 0;
      }
      ptr_1++;
    }
    ptr++;
  }
  return -1;
}

int main() {
  char input[50];

  cin.get(input, 50);
  cout << "Количество слов, оканчивающих на буквы a: "
       << count_a(input) // Выполнение задания 1
       << endl;
  cout << "Длина самого короткого слова: " << find_shortest(input)
       << endl; // Выполнение задания 2
  cout << "Количество букв b во втором слове: " << count_b(input)
       << endl; // Выполнение задания 3
  cout << "Строка в нижнем регистре: ";
  lower(input); // Выполнение задания 4
  cout << "Количество слов у которых первая и последняя буква совпадают: "
       << count_pal(input) << endl; // Выполнение задания 5
  cout << "Кратчайшая подстрока: "; // Выполнение задания 6
  find_str(input);
}