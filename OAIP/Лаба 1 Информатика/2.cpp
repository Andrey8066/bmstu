#include <iostream>

using namespace std;

int main() {
  int car, res;
  
  cout << "Ведите количество произведенных машин:\n";
  cin >> car;

  res = 1;

  while (car > 0)
  {
    res*=car;
    car-=1;
  }
  cout << "Факториал: " << res << endl;
  return 0;
}