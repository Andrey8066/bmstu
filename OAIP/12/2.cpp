#include <iostream>

using namespace std;

class Shape {
    public:
    int x;
    Shape(int px) : x(px)
    {}
    virtual int calculateArea()
    {
        return x;
    }
    virtual int calculatePerimeter()
    {
        return x;
    }
    virtual void dispclayINfo()
    {
        cout << x << calculateArea() << calculatePerimeter();
    }
};
class Rectangle : public Shape {
    public:
    int y;
    Rectangle(int px, int py) : Shape(px), y(py){}
    int calculateArea() override
    {
        return x*y;
    }
    int calculatePerimeter() override
    {
        return 2*(x+y);
    }
    void dispclayINfo() override
    {
        cout << x << y << calculateArea() << calculatePerimeter();
    }
};
int main ()
{
    Shape s(2);
    Rectangle r(2, 5);
    s.dispclayINfo();
    r.dispclayINfo();
    return 0;
}