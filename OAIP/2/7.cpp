#include <iostream>
using namespace std;
int main()
{
    int n = 10;
    int a[] = {1, 1, 4, 12, 56, 2, 23, 0, 10, 7};
    int auniq[n];
    auniq[0] = a[0];
    int min= 10000;
    int sum = 0;
    int c = 1;
    int indmax, indmin, indstart, indfinish;

    int res = 0;
    for (int i = 1; i<n; i++)
    {
                    bool f = true;

        for (int j = 0; j<n; j++)
        {
            if (a[i] == auniq[j])
            {
                f = false;
                
            }
            if (f)
            {
                auniq[c] = a[i];
                c+=1;
                break;
            }
        }    

    }
     for (int i = 0; i<n; i++)
    {
        cout << auniq[i] <<" ";
    } 
    cout << res;
}