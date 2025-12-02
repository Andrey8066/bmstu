#include <fstream>
#include <iostream>
#include <string>

using namespace std;

struct DATA {
  string name;
  int cost;
  int count;
};

DATA split(string line) {  // Разделение записи на даннные для структуры
  DATA d;
  int ptr = 0;
  string count;
  string cost;

  while (line[ptr] != ';') {
    d.name += line[ptr];
    ptr++;
  }
  ptr++;
  while (line[ptr] != ';') {
    cost += line[ptr];
    ptr++;
  }
  ptr++;
  while (line[ptr] != ';') {
    count += line[ptr];
    ptr++;
  }
  d.cost = stoi(cost);
  d.count = stoi(count);
  return d;
}

void read(DATA *products) { // Чтение данных из файла в структуру
  DATA *ptr = products;
  string line;
  ifstream ifStream("products.txt");
  while (getline(ifStream, line)) {

    DATA d = split(line);
    *ptr = d;
    ptr++;
  }
  ifStream.close();
}
void sort(DATA *products) {  // сортировка всех продуктов
  DATA replace;
  for (int i = 0; i < sizeof(products); i++) {
    int j = 0;
    while (products[j + 1].name != "") {
      if (products[j].name[0] > products[j + 1].name[0]) {
        replace = products[j];
        products[j] = products[j + 1];
        products[j + 1] = replace;
      }
      j++;
    }
  }
}
void show(DATA *products) {  // Вывод записей на экран
  int j = 0;
  while (products[j].name != "") {
    cout << "Продукт: " << products[j].name << ", его цена " << products[j].cost
         << ", его колличество в магазине " << products[j].count << endl;
    j++;
  }
}

int find(DATA *products, string f) { // Поиск среди продуктов
  int j = 0;
  while (products[j].name != "") {
    if (products[j].name == f) {
      cout << "Найден продукт: " << products[j].name << ", его цена "
           << products[j].cost << ", его колличество в магазине "
           << products[j].count << endl;
      return 0;
    }
    j++;
  }
  cout << "Такого продукта нет" << endl;
  return -1;
}
void filtrb(DATA *products, int cost) {  // Фильтрация продуктов дороже данной цены
  int j = 0;
  cout << "Вот все продукты с подходящей ценой:" << endl;
  while (products[j].name != "") {
    if (products[j].cost > cost)
      cout << "Продукт: " << products[j].name << ", его цена "
           << products[j].cost << ", его колличество в магазине "
           << products[j].count << endl;
    j++;
  }
}
void filtrm(DATA *products, int cost) {  // Фильтрация продуктов дешевле данной цены
  int j = 0;
  cout << "Вот все продукты с подходящей ценой:" << endl;
  while (products[j].name != "") {
    if (products[j].cost < cost)
      cout << "Продукт: " << products[j].name << ", его цена "
           << products[j].cost << ", его колличество в магазине "
           << products[j].count << endl;
    j++;
  }
}

void menu() {
  cout << "Выберите что хотите сделать;\n1. new  - Добавить новый продкукт\n2. "
          "show - Показать все продукты в порядке добавления\n3. sort - "
          "Показать все продукты отсортированными в алфавитном порядке\n4. "
          "find - Найти продукт по названию\n5. filtr - Показать продукты "
          "только с ценой больше/меньше данной"
       << endl;
}
int main() {
  menu();
  string a;
  cin >> a;
  while (a != "exit") {  // Ожидание комманды для завершения программы
    if (a == "new") {   // Добавление нового продукта
      DATA d;
      cout << "Введите название нового продукта: ";
      cin >> d.name;
      cout << "Введите цену нового продукта: ";
      cin >> d.cost;
      cout << "Введите колличество нового продукта: ";
      cin >> d.count;

      ofstream ofStream("products.txt", ios::app);
      ofStream << d.name << ";" << d.cost << ";" << d.count << ";" << endl;
      ofStream.close();
      cout << "Добавлен продукт " << d.name << ", его цена " << d.cost
           << ", колличество в магазине " << d.count << endl;

    } else if (a == "show") {  // Вывод всех строк из файла

      DATA r[100];
      read(r);
      show(r);

    } else if (a == "sort") {   // Сортировка записей

      DATA r[100];
      read(r);
      sort(r);
      show(r);
    } else if (a == "find") {  // Поиск в файле
      DATA r[100];
      
      string name;
      read(r);
      cout << "Введите название продукта который хотите найти" << endl;
      cin.ignore();
      getline(cin, name);
      find(r, name);
    } else if (a == "filtr") {  // Фильтрация продуктов
      DATA r[100];
      int f;
      char z;
      read(r);
      cout << "Введите значение относительно которого необходиимо фильтровать данные" << endl;
      cin >> f;
      cout << "Продукт должен быть дороже или дешевле?(>, <)" << endl;
      cin >> z;
      if (z == '>') filtrb(r, f);
      if (z == '<') filtrm(r, f);
    }
    menu();
    cin >> a;
  }
}