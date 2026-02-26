#pragma once

#include "Student.hpp"
#include <vector>
class Group {
private:
  std::string name;
  std::vector<std::shared_ptr<Student>> students;

public:
  void addStudent(Student gStudent);
  void removeStudentByName(std::string name);
  void print();
  std::vector<std::shared_ptr<Student>> getAllStudents();
  double getAverage();
  std::vector<std::shared_ptr<Student>> getSortedStudents();
  std::vector<std::shared_ptr<Student>> getStudentsByAverage(double average);
};