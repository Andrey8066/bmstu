#include <iostream>
using namespace std;
int main()
{
    int n = 10;
    int a[] = {9, 3, 4, 1, 56, 2, 23, 5, 10, 7};
    int min= 10000;
    int max = 0;
    int indmax, indmin;

    int res = 0;
    for (int i = 0; i<n; i++)
    {
        if (a[i]>max){
            max = a[i];
            indmax = i;
        }
        if (a[i]<min){
            min = a[i];
            indmin = i;
        }
    }
    if ( indmin == indmax-1)
    {
        cout<<a[indmax]+a[indmin];
    }
    return 0;
}