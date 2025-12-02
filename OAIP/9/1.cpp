#include <iostream>
#include <string>
class Person {
protected:
std::string name;
int age;
public:
void setInfo(const std::string& n, int a) {
name = n;
age = a;
}
void displayInfo() const {
std::cout << "Name: " << name << ", Age: " << age << std::endl;
}
};
class Student : public Person {
public:
int studentId;
void setStudentInfo(const std::string& n, int a, int id) {
name = n; // Доступ к защищенному атрибуту из производного класса
age = a; // Недоступно из производного класса, так как это private в базовом классе
studentId = id;
}
void displayStudentInfo() const {
std::cout << "Student ID: " << studentId << std::endl;
}
};
int main() {
// Создание объектов классов Person и Student и использование их методов
Person person1;
person1.setInfo("Alice", 25);
person1.displayInfo();
Student student1;
student1.setStudentInfo("Bob", 20, 12345);
student1.displayInfo(); // Используется метод базового класса
student1.displayStudentInfo();
return 0;
}