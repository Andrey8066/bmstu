#include <iostream>
using namespace std;
int main()
{
    int n = 10;
    int coords[][2] = {{1, 2}, {3, 4}, {5, 8}};
    int res = 0;


    res = 0.5 * abs(coords[0][0]*(coords[1][1]-coords[2][1]) + coords[1][0]*(coords[2][1]-coords[0][1]) + coords[2][0]*(coords[0][1]-coords[1][1]) );
    cout << coords[0][0]*(coords[1][1]-coords[2][1])<< endl;
    cout << coords[1][0]*(coords[2][1]-coords[0][1])<< endl;
    cout << coords[2][0]*(coords[0][1]-coords[1][1])<< endl;
    cout << abs(coords[0][0]*(coords[1][1]-coords[2][1]) + coords[1][0]*(coords[2][1]-coords[0][1]) + coords[2][0]*(coords[0][1]-coords[1][1]));
}

//-4+6+