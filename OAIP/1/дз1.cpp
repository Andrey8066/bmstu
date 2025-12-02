#include <iostream>
using namespace std;
int main()
{
    int n, min1, min2;
    cin >> n;
    int a[n];
    min1 = 100000;
    min2 = 100000;
    for (int i=0 ; i<n; i++)
    {
        cin >> a[i];
        if (min1>a[i])
        {

            min2 = min1;
            min1 = a[i];
        }
    }
    cout << min1 << min2;
}