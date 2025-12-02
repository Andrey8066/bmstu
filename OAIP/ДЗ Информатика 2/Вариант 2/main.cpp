#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <memory>
#include <pqxx/pqxx>
#include <string>

using namespace std;
using namespace pqxx;

class Sale { // Класс для работы с продажами
protected:
  int saleId;
  string saleDate;
  int productId;
  int customerId;
  int quantity;
  float amount;

public:
  Sale(int si, string sd, int pi, int ci, int q, float a) // Конструктор
      : saleId(si), saleDate(sd), productId(pi), customerId(ci), quantity(q),
        amount(a){};
  string getSale() { // Получение данных класса
    string res = to_string(saleId) + ", '" + saleDate + "', " +
                 to_string(productId) + ", " + to_string(customerId) + ", " +
                 to_string(quantity) + ", " + to_string(amount);
    return res;
  }
  string getDate() { // Преобразование даты
    string res = to_string(saleId) + ", (EXTRACT(YEAR FROM DATE '" +
                 to_string(saleDate) + "')), (EXTRACT(QUARTER FROM DATE '" +
                 to_string(saleDate) + "')), (EXTRACT(MONTH FROM DATE '" +
                 to_string(saleDate) + +"')), (EXTRACT(DAY FROM DATE '" +
                 to_string(saleDate) + "')), (TO_CHAR('" + to_string(saleDate) +
                 "'::date, 'DAY'))";
    return res;
  }
};

class Product { // Класс для работы с продуктами
protected:
  int productId;
  string productName;
  string category;
  float price;

public:
  Product(int pi, string pn, string c, float p) // Конструктор
      : productId(pi), productName(pn), category(c), price(p){};
  string getProduct() { // Получение данных о продукте
    string res = to_string(productId) + ", '" + productName + "', " + "'" +
                 category + "', " + to_string(price);
    return res;
  }
};

class Customer { // Класс для работы с покупателями
protected:
  int customerId;
  string customerName;
  string region;

public:
  Customer(int ci, string cn, string r) // Конструктор 
      : customerId(ci), customerName(cn), region(r){};
  string getCustomer() { // Получение данных о покупателе
    string res = to_string(customerId) + ", '" + customerName + "', " + "'" +
                 region + "'";
    return res;
  }
};

class CsvSales { // Класс для работы с файлом продаж
protected:
  string fileName;
  list<shared_ptr<Sale>> sales; // Список указателей на объекты класса Sale

public:
  CsvSales(string fn) : fileName(fn) {} // Конструктор

  void readFile() { // Получение данных из файла
    ifstream input(fileName);
    string line;
    while (getline(input, line)) {
      int saleId;
      string saleDate;
      int productId;
      int customerId;
      int quantity;
      float amount;

      int lptr = 0;
      int ptr = 0;
      string inf;

      while (line[ptr] != ';') {
        inf += line[ptr];
        ptr++;
      }
      saleId = stoi(inf);
      lptr = ptr + 1;
      ptr++;
      inf = "";
      while (line[ptr] != ';') {
        inf += line[ptr];
        ptr++;
      }
      saleDate = inf;
      lptr = ptr + 1;
      ptr++;
      inf = "";
      while (line[ptr] != ';') {
        inf += line[ptr];
        ptr++;
      }
      productId = stoi(inf);
      lptr = ptr + 1;
      ptr++;
      inf = "";
      while (line[ptr] != ';') {
        inf += line[ptr];
        ptr++;
      }
      customerId = stoi(inf);
      lptr = ptr + 1;
      ptr++;
      inf = "";
      while (line[ptr] != ';') {
        inf += line[ptr];
        ptr++;
      }
      quantity = stoi(inf);
      lptr = ptr + 1;
      ptr++;
      inf = "";
      while (line[ptr] != ';') {
        inf += line[ptr];
        ptr++;
      }
      amount = stof(inf);

      sales.push_back(make_shared<Sale>(saleId, saleDate, productId, customerId,
                                        quantity, amount));
      cout << sales.back()->getSale() << endl;
    }
  }

  void saveToDB() { // Сохранение данных в базу данных
    connection *c = new connection(
        "dbname=dwh user=postgres password=123456 hostaddr=127.0.0.1 "
        "port=5432");

    for (auto it = sales.begin(); it != sales.end(); ++it) {
      try {
        work *w = new work(*c);
        w->exec(" INSERT INTO sales_fact VALUES(" + (*it)->getSale() +
                "); INSERT INTO time_dim VALUES(" + (*it)->getDate() + ");");
        w->commit();
      } catch (const exception &e) {
      }
    }

    c->close(); // Отключение от базы данных
    delete c;
  }
};

class CsvProducts { // Класс для работы с файлом продуктов
protected:
  string fileName;
  list<shared_ptr<Product>> products;

public:
  CsvProducts(string fn) : fileName(fn) {} // Конструктор

  void readFile() { // Чтение данных о продуктах из файла
    ifstream input(fileName);
    string line;
    while (getline(input, line)) {
      int productId;
      string productName;
      string category;
      float price;

      int lptr = 0;
      int ptr = 0;
      string inf;

      while (line[ptr] != ';') {
        inf += line[ptr];
        ptr++;
      }
      productId = stoi(inf);
      lptr = ptr + 1;
      ptr++;
      inf = "";
      while (line[ptr] != ';') {
        inf += line[ptr];
        ptr++;
      }
      productName = inf;
      lptr = ptr + 1;
      ptr++;
      inf = "";
      while (line[ptr] != ';') {
        inf += line[ptr];
        ptr++;
      }
      category = inf;
      lptr = ptr + 1;
      ptr++;
      inf = "";
      while (line[ptr] != ';') {
        inf += line[ptr];
        ptr++;
      }
      price = stof(inf);

      products.push_back(
          make_shared<Product>(productId, productName, category, price));
    }
  }
  void saveToDB() { // Сохранение данных о продуктах в файл
    connection *c = new connection(
        "dbname=dwh user=postgres password=123456 hostaddr=127.0.0.1 "
        "port=5432");

    for (auto it = products.begin(); it != products.end();
         ++it) { // Вывод изменённого списка другим способом
      try {
        work *w = new work(*c);
        w->exec("BEGIN TRANSACTION; INSERT INTO products_dim VALUES(" +
                (*it)->getProduct() + "); COMMIT;");
        w->commit();
      } catch (const exception &e) {
      }
    }

    c->close(); // Отключение от базы данных
    delete c;
  }
};

class CsvCustomers { // Класс для работы с файлом покупателей
protected:
  string fileName;
  list<shared_ptr<Customer>> customers;

public:
  CsvCustomers(string fn) : fileName(fn) {} // Конструктор

  void readFile() { // Чтение данных о покупателях из файла 
    ifstream input(fileName);
    string line;
    while (getline(input, line)) {
      int customerId;
      string customerName;
      string region;

      int lptr = 0;
      int ptr = 0;
      string inf;

      while (line[ptr] != ';') {
        inf += line[ptr];
        ptr++;
      }
      customerId = stoi(inf);
      lptr = ptr + 1;
      ptr++;
      inf = "";
      while (line[ptr] != ';') {
        inf += line[ptr];
        ptr++;
      }
      customerName = inf;
      lptr = ptr + 1;
      ptr++;
      inf = "";
      while (line[ptr] != ';') {
        inf += line[ptr];
        ptr++;
      }
      region = inf;

      customers.push_back(
          make_shared<Customer>(customerId, customerName, region));
    }
  }
  void saveToDB() { // Сохранение данных о покупателях в файл
    connection *c = new connection(
        "dbname=dwh user=postgres password=123456 hostaddr=127.0.0.1 "
        "port=5432");

    for (auto it = customers.begin(); it != customers.end(); ++it) {
      try {
        work *w = new work(*c);
        w->exec("BEGIN TRANSACTION; INSERT INTO customers_dim VALUES(" +
                (*it)->getCustomer() + "); COMMIT;");
        w->commit();
      } catch (const exception &e) {
      }
    }
    c->close(); // Отключение от базы данных
    delete c;
  }
};

class Datawork { // Класс для работы с данными
public:
  Datawork(string cf, string pf, string sf) { // Конструктор
    CsvCustomers *cc = new CsvCustomers(cf);
    cc->readFile();
    cc->saveToDB();
    CsvProducts *cp = new CsvProducts(pf);
    cp->readFile();
    cp->saveToDB();
    CsvSales *cs = new CsvSales(sf);
    cs->readFile();
    cs->saveToDB();
  }
  void getSumByCategory() { // Получение колличества проданных товаров по категориям
    connection *c = new connection(
        "dbname=dwh user=postgres password=123456 hostaddr=127.0.0.1 "
        "port=5432");
    work *w = new work(*c);
    for (auto [category, sum] : // Вывод всех полученных их таблицы строк
         w->stream<string, float>(
             "SELECT p.category, SUM(s.amount) AS total_sales FROM sales_fact "
             "s JOIN products_dim p ON s.product_id = p.product_id GROUP BY "
             "p.category ORDER BY total_sales DESC"))
      cout << category << " : " << sum << endl;
    w->commit(); // Завершение транзакции
    c->close();  // Отключение от базы данных
    delete c;    // Очистка памяти]
    delete w;
  }
  void getNumbersByRegion() { // Получение количества продаж по регионам
    connection *c = new connection(
        "dbname=dwh user=postgres password=123456 hostaddr=127.0.0.1 "
        "port=5432");
    work *w = new work(*c);
    for (auto [category, sum] : // Вывод всех полученных их таблицы строк
         w->stream<string, float>(
             "SELECT c.region, COUNT(s.sale_id) AS number_of_sales FROM "
             "sales_fact s JOIN customers_dim c ON s.customer_id = "
             "c.customer_id GROUP BY c.region ORDER BY number_of_sales DESC"))
      cout << category << " : " << sum << endl;
    w->commit(); // Завершение транзакции
    c->close();  // Отключение от базы данных
    delete c;    // Очистка памяти]
    delete w;
  }
  void getAverageCost() { // ПОлучение среднего чека
    connection *c = new connection(
        "dbname=dwh user=postgres password=123456 hostaddr=127.0.0.1 "
        "port=5432");
    work *w = new work(*c);
    for (auto [year, month, cost] : // Вывод всех полученных их таблицы строк
         w->stream<int, int , float>(
             "SELECT t.year, t.month, AVG(s.amount) AS average_ticket FROM "
             "sales_fact s JOIN time_dim t ON s.sale_id = t.time_id GROUP BY "
             "t.year, t.month ORDER BY t.year, t.month"))
      cout << year << " : " << month << " : " << cost << endl;
    w->commit(); // Завершение транзакции
    c->close();  // Отключение от базы данных
    delete c;    // Очистка памяти]
    delete w;
  }
};

int main() {
  Datawork *d = new Datawork("customers.csv", "products.csv", "sales.csv");
  d->getSumByCategory();
  d->getNumbersByRegion();
  d->getAverageCost();

  return 0;
}