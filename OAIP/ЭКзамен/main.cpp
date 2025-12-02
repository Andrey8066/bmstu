#include <ctime>
#include <iostream>
#include <memory>
#include <pqxx/pqxx>
#include <string>
#include <fstream>

using namespace pqxx;
using namespace std;

void logger(string log_info){
    ofstream log = ofstream("./log");
    log << log_info << " : "  << time_t(0) << endl;
}

class DatabaseConnection { // Класс для работы с базой данных
protected:
  shared_ptr<connection> c; // Указатель на подключение к базе данных
  shared_ptr<work> w; // Указатель на транзакцию
  int status;

public:
  DatabaseConnection(string conn) {
    c = make_shared<connection>(conn);
  } // Конструктор класса, создающий подключение к БД
  ~DatabaseConnection() {
    c->close();
  } // Деструктор класса, закрывающий подключение
  void beginTransaction() { // Функция инициализирующая транзакцию
    w = make_shared<work>(*c);
    status = 1;
  }
  void commitTransaction() { // Функция завершающая транзакцию
    try {
      w.get()->commit();
      status = 2;
    } catch (const exception &e) { // Вывод ошибки
      cout << e.what() << endl;
      rollbackTransaction(); // Откат транзакции при ошибке
    }
  }
  void rollbackTransaction() { // Функция отменяющая изменения транзакции
    w.get()->abort();
    status = 3;
  }
  void executeNonQuery(
      string request) { // Выполнение запроса без возвращаемых данных
    try {
      w.get()->exec(request);
    } catch (const exception &e) { // Вывод ошибки
      cout << e.what() << endl;
      rollbackTransaction();
    }
  }

  int executeQuery(
      string request) { // Выполнение запроса с возвращаемыми данными
    try {
      result r = w.get()->exec(request);
      if (r.size() < 1) {
        return 0;
      }
      for (int rownum = 0; rownum < r.size(); ++rownum) {
        for (int colnum = 0; colnum < r.size(); ++colnum) {
          cout << r[rownum][colnum] << "  ";
        }
        cout << endl;
      }
      return 1;
    } catch (const exception &e) { // Вывод ошибки
      cout << e.what() << endl;
      rollbackTransaction();
      return 0;
    }
  }
  string getTransactionStatus() { // Функция возвращающая статус транзакции
    switch (status) {
    case 1:
      return "beginned";
    case 2:
      return "commited";
    case 3:
      return "rolledback";
    }
  }
};

class User {
protected:
  shared_ptr<DatabaseConnection> d;

public:
  User(string conn) {
    d = make_shared<DatabaseConnection>(conn);
  } // Конструктор класса
  void createOrder(int id, string product_ids,
                   string product_quantities) { // Функция создающая заказ
    d->beginTransaction();
    d->executeNonQuery("CALL createorder(" + to_string(id) + ", ARRAY[" +
                       product_ids + "], ARRAY[" + product_quantities + "])");
    d->commitTransaction();
    logger("Пользователь создал новый заказ");
  }
  void viewOrderStatus(int id) { // Функция выводящая список заказов
    d->beginTransaction();
    d->executeQuery("SELECT status FROM orders WHERE id = " + to_string(id));
    d->commitTransaction();
    logger("Пользователь посмотрел статус заказа");
  }
  void cancelOrder(int id) { // Функция отменяющая заказ
    d->beginTransaction();
    d->executeNonQuery(
        "UPDATE orders SET status = 'canceled' WHERE order_id = " +
        to_string(id));
    d->commitTransaction();
    logger("Пользователь отменил заказ");
  }
  int checkUser(string role) { // Функция проверяющая существование пользователя
    string email, password;
    cout << "Введите email пользователя: ";
    cin >> email;
    cout << "Введите пароль: ";
    cin >> password;
    d->beginTransaction();
    int res = d->executeQuery("SELECT * FROM users WHERE email = '" + email +
                              "'AND password_hash = TEXT(sha256('" +
                              (password) + "')) AND role = '" + role + "'");
    d->commitTransaction();
    if (res) logger("Пользователь вошел в аккаунт " + role);
    else logger("Пользователь не смог войти в аккаунт" + role);
    return res;
  }
  void createUser(string role) { // Функция создающая нового пользователя
    string email, password, name;
    cout << "Введите email пользователя: ";
    cin >> email;
    cout << "Введите пароль: ";
    cin >> password;
    cout << "Введите имя пользователя: ";
    cin >> name;
    d->beginTransaction();
    d->executeNonQuery(
        "INSERT INTO users (name, email, role, password_hash) VALUES ('" +
        name + "', '" + email + "', '" + role + "', sha256('" + password +
        "'))");
    d->commitTransaction();
    logger("Создан новый пользователь");
  }
};

class Admin : public User {
public:
  Admin(string conn) : User(conn){}; // Конструктор класса Admin
  void addProduct(string name, float price,
                  int count) { // Функция добавляющая новый продукт
    d.get()->beginTransaction();
    d.get()->executeNonQuery(
        "INSERT INTO products (name, price, stock_quantity) VALUES ('" + name +
        "', " + to_string(price) + ", " + to_string(count) + ")");
    d.get()->commitTransaction();

    logger("Администратор добавил новый продукт");
  }
  void updateProduct(int id, string name, float price,
                     int count) { // Функция обновляющая информацию о продукте
    d->beginTransaction();
    d->executeNonQuery("UPDATE products SET name = '" + name +
                       "', price = " + to_string(price) +
                       ", stock_quantity = " + to_string(count) +
                       " WHERE product_id = " + to_string(id));
    d->commitTransaction();

    logger("Администратор обновил продукт");
  }
  void deleteProduct(int id) { // Функция удаляющая продукт из БД
    d->beginTransaction();
    d->executeNonQuery("DELETE FROM products WHERE product_id = " +
                       to_string(id));
    d->commitTransaction();
    logger("Администратор удалил продукт");
  }
  void viewAllUsers() { // Функция выводящая список всех пользователей
    d->beginTransaction();
    d->executeQuery("SELECT name, email FROM users");
    d->commitTransaction();
    logger("Администратор посмотрел список всех пользователей");
  }
  void viewAllOrders() { // Функция выводящая список всех заказов
    d->beginTransaction();
    d->executeQuery("SELECT * FROM orders");
    d->commitTransaction();
    logger("Администратор посмотрел список всех заказов");
  }
  void updateOrderStatus(int id,
                         string status) { // Функция обновляющая статус заказа
    d->beginTransaction();
    d->executeNonQuery("UPDATE products SET status = " + status +
                       " WHERE order_id = " + to_string(id));
    d->commitTransaction();
    logger("Администратор обновил статус заказа");
  }
  int getMenu() { // Функция выводящая список команд доступных пользователю
    while (1) {
      char input;
      int id;
      string name;
      float price;
      int count;
      string status;
      cout << "1. Добавить новый продукт\n2. Обновить информацию о продукте\n"
              "3. "
              "Удалить продукт\n4. Просмотр всех заказов\n5. Просмотр всех "
              "пользователей\n6. Изменить статус заказа \n0. Выход\n";
      cin >> input;
      switch (input) {
      case '0':
        return 0;
      case '1':
        cout << "Введите название продукта который необходимо добавить: ";
        cin >> name;
        cout << "Введите цену продукта который необходимо добавить: ";
        cin >> price;
        cout << "Введите колличество продукта который необходимо добавить: ";
        cin >> count;
        addProduct(name, price, count);
        break;
      case '2':
        cout << "Введите id продукта который необходимо изменить: ";
        cin >> id;
        cout << "Введите название продукта который необходимо изменить: ";
        cin >> name;
        cout << "Введите цену продукта который необходимо изменить: ";
        cin >> price;
        cout << "Введите колличество продукта который необходимо изменить: ";
        cin >> count;
        updateProduct(id, name, price, count);
        break;
      case '3':
        cout << "Введите id продукта который необходимо удалить: ";
        cin >> id;
        deleteProduct(id);
        break;
      case '4':
        viewAllOrders();
        break;
      case '5':
        viewAllUsers();
        break;
      case '6':
        cout << "Введите id заказа который необходимо изменить: ";
        cin >> id;
        cout << "Введите статус заказа который необходимо изменить: ";
        cin >> status;
        updateOrderStatus(id, status);
        break;
      default:
        return 0;
      }
    }
  }
};

class Manager : public User {
public:
  Manager(string conn) : User(conn){}; // Конструктор класса Manager
  void approveOrder(int id) { // Функция подтверждения заказа
    d->beginTransaction();
    d->executeQuery("");
    d->commitTransaction();
    logger("Менеджер подтвердил заказ");
  }
  void updateStock(int id,
                   int count) { // Функция обновляющая количество продукта
    d->beginTransaction();
    d->executeQuery("UPDATE products SET stock_quantity = (SELECT "
                    "stock_quantity FROM products WHERE product_id = " +
                    to_string(id) + ") + " + to_string(count) +
                    " WHERE product_id = " + to_string(id));
    d->commitTransaction();
    logger("Менеджер обновил колличество продукта");
  }
  void addToOrder(int id, string product_ids,
                  string product_quantities) { // // Функция добавляющая новые
                                               // продукты в заказ
    d->beginTransaction();
    d->executeQuery("CALL createorder(" + to_string(id) + ", ARRAY[" +
                    product_ids + "], ARRAY[" + product_quantities + "])");
    d->commitTransaction();
    logger("Менеджер добавил товары в заказ");
  }
  int getMenu() { // Функция выводящая список команд доступных пользователю
    while (1) {
      char input;
      int id;
      int count;
      string product_ids, product_quantities;
      cout << "1. Утвердить заказ\n2. Обновить запасы товаров на скаде\n3. "
              "Добавить товары в заказ\n0.Выход\n";
      cin >> input;
      switch (input) {
      case '0':
        return 0;
      case '1':
        int input;
        cout << "Введите номер заказа для подтверждения" << endl;
        cin >> input;
        approveOrder(input);
        break;
      case '2':
        cout << "Введите id продукта количество которого необходимо "
                "изменить: ";
        cin >> id;
        cout << "Введите число на которое изменилось колличество продуктов: ";
        cin >> count;
        updateStock(id, count);
        break;
      case '3':
        cout << "Введите id пользователя который сделал заказ: ";
        cin >> id;
        cout << "Введите id продуктов которые необходимо добавить в заказ: ";
        cin >> product_ids;
        cout << "Введите количество продуктов которые необходимо добавить: ";
        cin >> product_quantities;
        addToOrder(id, product_ids, product_quantities);
        break;
      default:
        return 0;
      }
    }
  }
};

class Customer : public User {
public:
  Customer(string conn) : User(conn){}; // Конструктор класса Customer

public:
  void addToOrder(int id, int product_id,
                  int quantity) { // Функция добавляющая новые продукты в заказ
    d->beginTransaction();
    d->executeQuery(
        "INSERT INTO order_items(order_id, product_id, quantity) VALUES (" +
        to_string(id) + ", " + to_string(product_id) + ", " +
        to_string(quantity) + ")");
    d->commitTransaction();
    logger("Покупатель добавил продукты в заказ");
  }
  void removeFromOrder(int id,
                       int product_id) { // Функция удаляющая продукт из заказа
    d->beginTransaction();
    d->executeQuery("DELETE FROM order_items WHERE order_id = " +
                    to_string(id) + ", product_id = " + to_string(product_id));
    d->commitTransaction();
    logger("Пользователь удалил продукты из заказа");
  }
  void makePayment(int id) { // Функция оплаты заказа
    d->beginTransaction();
    d->executeQuery("UPDATE orders SET price = 0 WHERE id = " + to_string(id));
    d->commitTransaction();
    logger("Пользователь оплатил заказа");
  }
  int getMenu() { // Функция выводящая список команд доступных пользователю
    while (1) {
      char input;
      int id, product_id, quantity;
      string product_ids, product_quantities;
      cout << "1. Создать новый заказ\n2. Добавить товар в заказ\n3. Удалить "
              "товар из заказа\n4. Оплатить заказ\n5. Просмотреть статус "
              "заказов\n0. Выход\n";
      cin >> input;
      switch (input) {
      case '0':
        return 0;
      case '1':
        cout << "Введите id заказа в который надо добавить продукты: ";
        cin >> id;
        cout << "Введите id продукта который необходимо добавить в заказ: ";
        cin >> product_ids;
        cout << "Введите колличество продукта который необходимо добавить в "
                "заказ: ";
        cin >> product_quantities;
        createOrder(id, product_ids, product_quantities);
        break;
      case '2':
        cout << "Введите id заказа в который надо добавить продукты: ";
        cin >> id;
        cout << "Введите id продукта который необходимо добавить в заказ: ";
        cin >> product_id;
        cout << "Введите колличество продукта который необходимо добавить в "
                "заказ: ";
        cin >> quantity;
        addToOrder(id, product_id, quantity);
        break;
      case '3':
        cout << "Введите id заказа из которого надо удалить продукты: ";
        cin >> id;
        cout << "Введите id продукта который необходимо удалить из заказа: ";
        cin >> product_id;
        removeFromOrder(id, product_id);
        break;
      case '4':
        cout << "Введите id заказа который желаете оплатить: ";
        cin >> id;
        makePayment(id);
        break;
      case '5':
        cout << "Введите id заказа статус которого желаете посмотреть: ";
        cin >> id;
        viewOrderStatus(id);
        break;
      default:
        return 0;
      }
    }
  }
};

int main() {
  while (1) {
    char input;
    string conn =
        "dbname=exam user=postgres password=123456 hostaddr=127.0.0.1 "
        "port=5432"; // Строка подключения к БД
    cout << "1. Войти как Администратор\n2. Войти как Менеджер\n3. Войти как "
            "Покупатель\n0. Выход\n"; 
    cin >> input; // Выбор роли и пользователя
    Admin *a = new Admin(conn);
    Manager *m = new Manager(conn);
    Customer *c = new Customer(conn);
    switch (input) { // Меню входа
    case '0':
      exit(0);
    case '1':
      if (a->checkUser("admin")) {
        cout << "Успешный вход" << endl;
        a->getMenu();
      } else
        cout << "Пользователь не найден" << endl;
      break;
    case '2':
      if (m->checkUser("manager")) {
        cout << "Успешный вход" << endl;
        m->getMenu();
      } else
        cout << "Пользователь не найден" << endl;
      break;
    case '3':
      if (c->checkUser("customer")) {
        cout << "Успешный вход" << endl;
        c->getMenu();
      } else {
        cout << "Пользователь не найден" << endl;
        c->createUser("customer");
      }
      break;
    default:
      exit(0);
    }
  }
}