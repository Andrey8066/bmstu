#include <iostream>
using namespace std;
int main()
{
    int n = 10;
    int a[] = {1, 3, 4, 12, 56, 2, 23, 0, 10, 7};
    int min= 10000;
    int sum = 0;
    int max = 0;
    int indmax, indmin, indstart, indfinish;

    int res = 0;
    for (int i = 0; i<n; i++)
    {
        sum += a[i];
        if (a[i]<min){
            min = a[i];
            indmin = i;
        }
    

    }
    a[indmin] = int(sum/n);
    for (int i = 0; i<n; i++)
    {
        cout << a[i] <<" ";
    } 
}