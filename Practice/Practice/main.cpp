
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <functional>

using namespace std;

template<typename InputIter, typename OutputIter, typename Opr>
void subVec(InputIter inputIterBegin, InputIter inputIterEnd, OutputIter outputIter, Opr opr)
{
    sort(inputIterBegin, inputIterEnd);

    InputIter vIter;
    //greater_equal<int>() is a function object
    vIter = find_if(inputIterBegin, inputIterEnd, opr);
    copy(inputIterBegin, vIter, outputIter);
}

int main()
{
    const int maxLen = 8;
    int arr[maxLen] = {3, 55, 21, 18, 8, 9, 78, 12};
    vector<int> vec(arr, arr+maxLen);
    vector<int> nVec(maxLen);
    int nArr[maxLen];
    subVec(vec.begin(), vec.end(), nVec.begin(), bind2nd(greater<int>(), 8));
    subVec(arr, arr+maxLen, nArr, bind2nd(less<int>(), 8));
    for (int i=0; i<maxLen; i++)
    {
        cout << nVec[i] << " ";
    }
    cout << endl;
    for (int i=0; i<maxLen; i++)
    {
        cout << nArr[i] << " ";
    }
    cout << endl;
    return 0;
}