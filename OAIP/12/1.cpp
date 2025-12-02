#include <iostream>

using namespace std;
class Publication {
    public:
    string name;
    Publication(const string n) : name(n)
    {}
    
};

class Book : public Publication{
    public:
    string author;
    int year;
    Book(string n, string a, int y): Publication(n), author(a), year(y) 
    {    }
    void dispclayINfo()
    {
        cout << name << author << year;
    }

};

int main ()
{
    Publication p("ДА");
    Book b("Мастер и маргарита", "Толстой", 1830);
    b.dispclayINfo();
    return 0;
}