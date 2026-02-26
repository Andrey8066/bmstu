#include "RecordBook.hpp"
#include <iostream>
#include <memory>

void RecordBook::addMark(Mark mark) {
  marks.emplace_back(std::make_shared<Mark>(mark));
}
double RecordBook::getAverage() {
  double res = 0;
  for (std::shared_ptr<Mark> mark : marks) {
    res += mark->grade;
  }
  return res;
}
void RecordBook::print() {
  std::cout << "Зачетная книжка №" << recordBookNumber << " со средним баллом"
            << getAverage() << std::endl;
}
RecordBook::RecordBook(int number) { recordBookNumber = number; }