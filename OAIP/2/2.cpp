#include <iostream>
using namespace std;
int main()
{
    int n = 10;
    int coords[][2] = {{1, 2}, {3, 4}, {5, 6}, {7, 8}, {19, 20}, {9, 10}, {11, 12}, {13, 14}, {15, 16}, {17, 18}};
    int min= 10000;
    int max = 0;
    int indmax, indmin, indstart, indfinish;

    int res = 0;
    for (int i =0; i<n; i++)
    {
        for (int j = 0; j<n; j++)
        {
            if ((coords[i][0] - coords[j][0])*(coords[i][0] - coords[j][0]) + (coords[i][1]-coords[j][1])*(coords[i][1]-coords[j][1])> max)
            {
                max = (coords[i][0] - coords[j][0])*(coords[i][0] - coords[j][0]) + (coords[i][1]-coords[j][1])*(coords[i][1]-coords[j][1]) ;
                indstart = i;
                indfinish = j;
            }
        
        cout << indfinish << " " <<indstart <<  " " << max << endl;
        }
    }
    cout << indfinish << indstart;
}