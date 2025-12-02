#include <iostream>
#include <string>

using namespace std;
class Publication {
public:
  string title;

public:
  Publication(const string s) : title(s) {}
  virtual void dispclayInfo() { cout << title << endl; }
};
class Book : public Publication {
    protected:
    string author;
    int year;
    public:
    Book(const string s, const string a, int y) : Publication(s), author(a), year(y) {}
    void dispclayInfo() override 
    {
        cout << title << author << year << endl;
    }
};

int main() { return 0; }