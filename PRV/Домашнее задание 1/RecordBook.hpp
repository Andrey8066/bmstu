#pragma once

#include <memory>
#include <string>
#include <vector>

struct Mark {
  std::string subject;
  double grade;
};

class RecordBook {
private:
  int recordBookNumber;
  std::vector<std::shared_ptr<Mark>> marks;

public:
  RecordBook(int number);
  void addMark(Mark mark);
  double getAverage();
  void print();
};