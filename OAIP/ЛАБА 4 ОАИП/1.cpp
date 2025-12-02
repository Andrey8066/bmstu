#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <pqxx/pqxx>

using namespace std;
using namespace pqxx;

void write_to_log(string logs) // Функция для логирования изменений базы данных
{
  ofstream logfile("operations.log", ios::app);
  time_t now = time(0); // Получение настояшего времени
  logfile << now << ": " << logs << endl; // Запись в logfile
}

class Category { // Класс для работы с таблтцей категорий товаров
protected:
  string params; // Настройки доступа к базе данных
public:
  Category(string p) : params(p) {} //  Конструктор класса
  void write(string category_name) { // Добавление записи в таблицу
    try {
      connection *c = new connection(params); // Соединение с базой данных
      work *w = new work(*c); // Cоздание транзакции
      result *r = new result(
          w->exec(" INSERT INTO categories (category_name) VALUES ('" +
                  category_name + "')"));
      w->commit(); // Завершение транзакции
      c->close();  // Отключение от базы данных
      delete c;    // Очистка памяти
      delete w;
      delete r;
      write_to_log("Произведена запись в таблицу Category('INSERT INTO "
                   "categories (category_name) VALUES ('" +
                   category_name + "')')"); // Запись в log-файл
    } catch (const exception &e) {          // Вывод ошибки
      cout << e.what() << endl;
    }
  }
  void read_all() { // Чтение всех записей из таблицы
    try {
      connection *c = new connection(params); // Соединение с базой данных
      work *w = new work(*c); // Cоздание транзакции
      for (auto [name] :
           w->stream<std::string_view>("SELECT category_name FROM categories"))
        cout << setw(10) << left << name << endl;

      w->commit(); // Завершение транзакции
      c->close();  // Отключение от базы данных
      delete c;    // Очистка памяти
      delete w;
    } catch (const exception &e) { // Вывод ошибки
      cout << e.what() << endl;
    }
  }
};

class Product { // Класс для работы с таблицей товаров
protected:
  string params; // Настройки доступа к базе данных
public:
  Product(string p) : params(p) {} //  Конструктор класса
  void write(string name, int price, int quantity,
             int category_id) { // Добавление записи в таблицу
    try {
      connection *c = new connection(
          params); // Соединение с базой данных // Соединение с базой данных
      work *w = new work(*c); // Cоздание транзакции
      result *r = new result(w->exec(" INSERT INTO products (name, price, "
                                     "quantity, category_id) VALUES ('" +
                                     name + "', " + to_string(price) + ", " +
                                     to_string(quantity) + ", " +
                                     to_string(category_id) + ")"));
      w->commit(); // Завершение транзакции

      c->close(); // Отключение от базы данных
      delete c;   // Очистка памяти
      delete w;
      delete r;
      write_to_log("Произведена запись в таблицу Product('INSERT INTO products "
                   "(name, price, quantity, category_id) VALUES ('" +
                   name + "', " + to_string(price) + ", " +
                   to_string(quantity) + ", " + to_string(category_id) +
                   "))");          // Запись в log-файл
    } catch (const exception &e) { // Вывод ошибки
      cout << e.what() << endl;
    }
  }
  void read_all() { // Чтение всех записей из таблицы
    try {
      connection *c = new connection(params); // Соединение с базой данных
      work *w = new work(*c); // Cоздание транзакции
      for (auto [name, price, quantity,
                 category_id] : // Вывод всех полученных их таблицы строк
           w->stream<std::string_view, float, int, int>(
               "SELECT name, price, quantity, category_id FROM products"))
        cout << setw(10) << name << " " << setw(10) << price << " " << setw(10)
             << quantity << " " << setw(10) << category_id << endl;

      w->commit(); // Завершение транзакции
      c->close();  // Отключение от базы данных
      delete c;    // Очистка памяти
      delete w;
    } catch (const exception &e) { // Вывод ошибки
      cout << e.what() << endl;
    }
  }
  void get_by_category(string category) // Получение из таблицы всех записей с
                                        // заданной категорией
  {
    try {
      connection *c = new connection(params); // Соединение с базой данных
      work *w = new work(*c); // Cоздание транзакции
      for (auto [name, price,
                 quantity] : // Вывод всех полученных их таблицы строк
           w->stream<std::string_view, float, int>(
               "SELECT name, price, quantity FROM products WHERE category_id = "
               "(SELECT id FROM categories WHERE category_name = '" +
               category + "')"))
        cout << setw(10) << name << " " << setw(10) << price << " " << setw(10)
             << quantity << " " << endl;
      w->commit(); // Завершение транзакции
      c->close();  // Отключение от базы данных
      delete c;    // Очистка памяти
      delete w;
    } catch (const exception &e) { // Вывод ошибки
      cout << e.what() << endl;
    }
  }
  void count_sales() // Подсчет продаж каждого продукта
  {
    try {
      connection *c = new connection(params); // Соединение с базой данных
      work *w = new work(*c); // Cоздание транзакции
      for (auto [name, sales] : // Вывод всех полученных их таблицы строк
           w->stream<string, int>(
               "SELECT (SELECT name FROM products WHERE id= product_id), "
               "SUM(quantity_sold) FROM sales GROUP BY product_id"))
        cout << setw(10) << name << " " << setw(10) << sales << " " << endl;
      w->commit(); // Завершение транзакции
      c->close();  // Отключение от базы данных
      delete c;    // Очистка памяти
      delete w;
    } catch (const exception &e) { // Вывод ошибки
      cout << e.what() << endl;
    }
  }
  void get_top_saled() // Получение из таблицы 5 самых продоваемых товаров
  {
    try {
      connection *c = new connection(params); // Соединение с базой данных
      work *w = new work(*c); // Cоздание транзакции
      for (auto [name, sales] : // Вывод всех полученных их таблицы строк
           w->stream<string, int>(
               "SELECT (SELECT name FROM products WHERE id = product_id), "
               "SUM(quantity_sold) FROM sales GROUP BY name ORDER BY "
               "SUM(quantity_sold) DESC LIMIT 5"))
        cout << setw(10) << name << " " << setw(10) << sales << " " << endl;

      w->commit(); // Завершение транзакцииc->close(); // Отключение от базы
                   // данных
      delete c;    // Очистка памяти
      delete w;
    } catch (const exception &e) { // Вывод ошибки
      cout << e.what() << endl;
    }
  }
};

class Sale { // Класс для работы с таблицей продаж
protected:
  string params; // Настройки доступа к базе данных
public:
  Sale(string p) : params(p) {} //  Конструктор класса
  void write(int product_id, string sale_date,
             int quantity_sold) { // Добавление записи в таблицу
    try {
      connection *c = new connection(params); // Соединение с базой данных
      work *w = new work(*c); // Cоздание транзакции
      result *r = new result(w->exec("UPDATE products SET quantity= (Select "
                                     "quantity From products Where id = " +
                                     to_string(product_id) + " ) -" +
                                     to_string(quantity_sold) +
                                     " Where id = " + to_string(product_id)));
      w->commit(); // Завершение транзакции
      delete r;
      delete w;
      w = new work(*c);
      r = new result(w->exec(
          " INSERT INTO sales (product_id, sale_date, quantity_sold) VALUES (" +
          to_string(product_id) + ", '" + sale_date + "', " +
          to_string(quantity_sold) + ")"));
      w->commit(); // Завершение транзакции
      c->close();  // Отключение от базы данных
      delete c;    // Очистка памяти
      delete r;
      delete w;
      write_to_log("Произведена запись в таблицу Sale('UPDATE products SET "
                   "quantity= (Select quantity From products Where id = " +
                   to_string(product_id) + " ) -" + to_string(quantity_sold) +
                   " Where id = " + to_string(product_id) +
                   "')"); // Запись в log-файл
      write_to_log("Произведена запись в таблицу Sale('INSERT INTO sales "
                   "(product_id, sale_date, quantity_sold) VALUES (" +
                   to_string(product_id) + ", '" + sale_date + "', " +
                   to_string(quantity_sold) + ")')"); // Запись в log-файл
    } catch (const exception &e) {                    // Вывод ошибки
      cout << e.what() << endl;
    }
  }
  void read_all() { // Чтение всех записей из таблицы
    try {
      connection *c = new connection(params); // Соединение с базой данных
      work *w = new work(*c); // Cоздание транзакции

      for (auto [product_id, sale_date,
                 quantity_sold] : // Вывод всех полученных их таблицы строк
           w->stream<int, std::string_view, int>(
               "SELECT product_id, sale_date, quantity_sold FROM sales"))
        cout << setw(10) << product_id << " " << setw(10) << sale_date << " "
             << setw(10) << quantity_sold << endl;
      w->commit(); // Завершение транзакции
      c->close();  // Отключение от базы данных
      delete c;    // Очистка памяти
      delete w;
    } catch (const exception &e) { // Вывод ошибки
      cout << e.what() << endl;
    }
  }
  void revenue() { // Подсчет выручки
    try {
      connection *c = new connection(params); // Соединение с базой данных
      work *w = new work(*c); // Cоздание транзакции
      for (auto [r] : // Вывод всех полученных их таблицы строк
           w->stream<float>("SELECT SUM(quantity_sold*(SELECT price FROM "
                            "products WHERE id = product_id)) FROM sales"))
        cout << "Выручка от всех продаж: " << r << endl;
      w->commit(); // Завершение транзакции
      c->close();  // Отключение от базы данных
      delete c;    // Очистка памяти // Очистка памяти
      delete w;
    } catch (const exception &e) { // Вывод ошибки
      cout << e.what() << endl;
    }
  }
};

class PhisycalProduct : public Product {};
class DigitalProduct : public Product {};

template <typename T> class Pair { // Шаблонный класс Pair
protected:
  T name;
  T quantity;

public:
  Pair(T n, T q) : name(n), quantity(q) {}
  T get_name() { return name; }
  T get_quantity() { return quantity; }
  void set_name(T n) { name = n; }
  void set_quantity(T q) { quantity = q; }
};

int main() {
  Product p(
      "dbname=products_db user=postgres password=123456 hostaddr=127.0.0.1 "
      "port=5432"); // Инициализация объекта p класса Product
  Category cat(
      "dbname=products_db user=postgres password=123456 hostaddr=127.0.0.1 "
      "port=5432"); // Инициализация объекта cat класса Category
  Sale s("dbname=products_db user=postgres password=123456 hostaddr=127.0.0.1 "
         "port=5432"); // Инициализация объекта s класса Sale
  cout << "Добавление категории Vegetables" << endl;
  cat.write("Vegetables"); // Пример использования метода write класса Category
  cout << "Чтение всех записе из таблицы Categories" << endl;
  cat.read_all(); // Пример использования метода read_all класса Category

  cout << "Добавление записи в таблицу Products" << endl;
  p.write("Cucumber", 20, 105,
          3); // Пример использования метода write класса Product
  cout << "Чтение всех записе из таблицы Categories" << endl;
  p.read_all(); // Пример использования метода read_all класса Product
  cout << "Чтение всех записей с категорией Vegetables  из таблицы Products"
       << endl;
  p.get_by_category("Vegetables"); // Пример использования метода
                                   // get_by_category класса Product
  cout << "Получение топ 5 самых продоваемых товаров" << endl;
  p.get_top_saled(); // Пример использования метода get_top_salled класса
                     // Product
  cout << "Вывод товаров и количества проданных товаров" << endl;
  p.count_sales(); // Пример использования метода count_sales класса Product

  cout << "Добавление записи о продаже в таблицу Sales" << endl;
  s.write(3, "06/12/2024", 8); // Пример использования метода write класса Sales
  cout << "Чтение всех записей из таблицы Sales" << endl;
  s.read_all(); // Пример использования метода readl_all класса Sale
  cout << "Подсчет выручки от всех продаж" << endl;
  s.revenue(); // Пример использования метода revenue класса Sale

  return 0;
}
