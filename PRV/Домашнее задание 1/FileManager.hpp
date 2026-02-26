#pragma once

#include "Group.hpp"

struct Header {};
class FileManager {
public:
  int saveGroup(Group group);
  Group loadGroup();
  int checkFileFormat();

private:
};