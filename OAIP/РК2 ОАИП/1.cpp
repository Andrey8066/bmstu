#include <iostream>
#include <list>
#include <memory>

using namespace std;
class Vehicle {
protected:
  string mark, model; // Характеристики базового класса

public:
  Vehicle(const string ma, const string mo) : mark(ma), model(mo) {} // Конструктор
  virtual void display() { // Виртуальная функция для вывода характеристик
    cout << "Марка транспортного средста: " << mark
         << ". Модель транспортного средства: " << model << "." << endl;
  }
  void set_info(const string ma, const string mo) { // Функция для изменения характеристик
    mark = ma;
    model = mo;
  }
};

class Car : public Vehicle {
protected:
  int number_of_doors; // Колличество дверей автомобиля

public:
  Car(const string ma, const string mo, int n)
      : Vehicle(ma, mo), number_of_doors(n) {} // Конструктор
  void display() override { // Невиртуальная функция для вывода характеристик
    cout << "Марка машины: " << mark << ". Модель машины: " << model
         << ". Колличество дверей: " << number_of_doors << "." << endl;
  }
};

class Motorcycle : public Vehicle { 
protected:
  const string type; //Тип мотоцикла

public:
  Motorcycle(const string ma, const string mo, const string t)
      : Vehicle(ma, mo), type(t) {} // Конструктор
  void display() override {  // Невиртуальная функция для вывода характеристик
    cout << "Марка машины: " << mark << ". Модель машины: " << model
         << ". Тип мотоцикла: " << type << "." << endl;
  }
};

void changeVehicle(Vehicle *vehicle, string mark, string model) {  // Функция для изменения характеристики для любого из производных классов
  vehicle->set_info(mark, model);
}
int main() {
  list<shared_ptr<Vehicle>> vehicles; // контейнер для хранения указателей на объекты классов Vehicle

  vehicles.push_back(make_shared<Car>("Ford", "Mustang", 3)); // Добавление машины Ford Mustang  с 3 дверями в контейнер для хранения указателей на объекты классов Vehicle
  vehicles.push_back(make_shared<Car>("Toyota", "Camry", 5)); // Добавление машины Toyota Camry с 5 дверями в контейнер для хранения указателей на объекты классов Vehicle
  vehicles.push_back(
      make_shared<Motorcycle>("Harley-Davidson", "Softail", "cruiser")); // Добавление мотоцикла Harley-Davidson Softail типа cruiser в контейнер для хранения указателей на объекты классов Vehicle
  vehicles.push_back(make_shared<Motorcycle>("Honda", "Gold Wing", "touring")); // Добавление мотоцикла Honda Gold Wing типа touring в контейнер для хранения указателей на объекты классов Vehicle

  for (const auto &vehicle : vehicles) { // Вывод исходного списка

    vehicle->display();
  }
  cout << endl;

  changeVehicle(vehicles.front().get(), "BMW", "E38"); // Изменение атрибутов базового класса для первого эллемента в списке

   
    for (auto it = vehicles.begin(); it != vehicles.end(); ++it) { // Вывод изменённого списка другим способом
        (*it)->display();
    }


  return 0;
}