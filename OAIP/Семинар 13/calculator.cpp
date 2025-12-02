#include <iostream>
#include <string>

using namespace std;
class calculator 
{
    public:
    calculator(const double pa, const double pb) : a(pa), b(pb) {}
    double sum()
    {
        return a+b;
    }
    double razn()
    {
        return a-b;
    }
    double delit()
    {
        return a/b;
    }
    double umnozh()
    {
        return a*b;
    }
    private:
    double a, b;
};

int main()
{
    double a, b;
    char* oper;
    string sa, sb;
    char deistv;
    cin >> oper;

    char *ptr = oper;


    while (*ptr != '*' && *ptr != '-' && *ptr != '+' && *ptr != '/' )
    {
        sa+=*ptr;
        ptr++;
    }
    deistv = *ptr;
    ptr++;

    while (*ptr != '\0')
    {
        sb+=*ptr;
        ptr++;
    }
    a = stod(sa);
    b = stod(sb);

    calculator c(a, b);

    switch (1; deistv)
    {
    case '*':
    {
        cout << c.umnozh() << endl;
        break;
    }
    case '+':
    {
        cout << c.sum() << endl;
        break;
    }
    case '-':
    {
        cout << c.razn() << endl;
        break;
    }
    case '/':
    {
        cout << c.delit() << endl;
        break;
    }
    }
    
    return 0;
}