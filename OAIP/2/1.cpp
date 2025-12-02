#include <iostream>
using namespace std;
int main()
{
    int n = 10;
    int a[] = {1, 3, 4, 12, 56, 2, 23, 0, 10, 7};
    int min= 10000;
    int max = 0;
    int indmax, indmin, indstart, indfinish;

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
    if (indmin>indmax) {
        indstart = indmax;
        indfinish = indmin;
    }
    else {

        indstart = indmin;
        indfinish = indmax;
    }

    for ( int x =indstart; x <= indfinish; x++)
    {
        res+= a[x];
    }
    cout << res << indstart << indfinish;
    return 0;
}