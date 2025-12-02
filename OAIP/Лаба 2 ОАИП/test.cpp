#include <iostream>

using namespace std;

int main()
{
    char input;
    
    cin >> input;

    cout << static_cast<char>(input+32);
    return 0;
}

