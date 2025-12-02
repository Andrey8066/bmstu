#include <iostream>
using namespace std;
int main() {
  int speed;

  cout << "Введите скорость: ";
  cin >> speed;

  if (speed < 60) cout << "Скорость в пределах нормы\n";
  else if (speed >= 60  && speed < 80) cout << "ВЫ приближаетесь к превышению скорости\n";
  else cout << "Превышение скорости\n";
  return 0;
}