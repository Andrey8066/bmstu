#include <iostream>

using namespace std;

int* sort(int n, int a[])
{
    for (int i = n; i<n; i--)
    {
        int j = 0;
        while (a[i-j]>a[i-j-1])
        {
            int s = a[i-j];
            a[i-j] = a[i-j-1];
            a[i-j-1] = s;
            j+=1;
        }
    }
    return a;
}

int main()
{
    int a[] = {3, 9, 10, 4, 5};
    int n = 5;
    for (int i = n; i<n; i--)
    {
        int j = 0;
        while (a[i-j]>a[i-j-1])
        {
            j+=1;
        }
        a2[i-]
    }
    cout<<a[0]<<a[1]<<a[2]<<a[3]<<a[4];
    return 0;
}

