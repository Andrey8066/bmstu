
#include "Group.hpp"
#include "Student.hpp"
#include <algorithm>
#include <memory>

using namespace std;

void Group::addStudent(Student gStudent) {
  students.emplace_back(make_shared<Student>(gStudent));
}
void Group::removeStudentByName(string name) {
  for (int i = 0; i < students.size(); i++) {
    if (students.at(i)->getName() == name) {
      students.erase(students.begin() + i);
      return;
    }
  }
}
void Group::print() {}
vector<shared_ptr<Student>> Group::getAllStudents() { return students; }
double Group::getAverage() {
  double res = 0;
  for (shared_ptr<Student> student : students) {
    res += student->getAverage();
  }
  return res / students.size();
}
vector<shared_ptr<Student>> Group::getSortedStudents() {
  vector<shared_ptr<Student>> sortedStudents = students;
  sort(sortedStudents.begin(), sortedStudents.end(),
       [](shared_ptr<Student> a, shared_ptr<Student> b) {
         return a->getAverage() < b->getAverage();
       });
  return sortedStudents;
}
vector<shared_ptr<Student>> Group::getStudentsByAverage(double average) {
  vector<shared_ptr<Student>> res;

  for (shared_ptr<Student> student : students) {
    if (student->getAverage() >= average) {
      res.emplace_back(make_shared<Student>(student));
    }
  }
  return res;
}