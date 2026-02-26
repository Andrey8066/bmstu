#include "Student.hpp"
#include "Person.hpp"
#include "RecordBook.hpp"
#include <iostream>

double Student::getAverage() { return recordBook.getAverage(); }

void Student::print() {
  std::cout << "Студент " << name << " со средним баллом " << getAverage()
            << std::endl;
}

void Student::addMark(Mark mark) { recordBook.addMark(mark); }

Student::Student(std::string pName, int rBNumber) : Person(pName) {
  recordBook = RecordBook(rBNumber);
}