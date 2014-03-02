
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>

using namespace std;
int main()
{
    vector<string> vStr;
    string str;
    fstream infile("E:\\GitHub\\Samples\\Practice\\Practice\\test.txt");
    if(!infile)
    {
        cerr << "Can not open file" << endl;
        return -1;
    }
    while (infile >> str)
    {
        //cout << str << " ";
        vStr.push_back(str);
    }
    for (int i = 0; i < vStr.size(); ++i)
    {
        cout << vStr[i] << " ";
    }
    sort(vStr.begin(), vStr.end());
    ofstream outfile("newFile.txt");
    if (!outfile)
    {
        cerr << "Can not open writable file." << endl;
        return -1;
    }
    for (int i=0; i < vStr.size(); ++i)
    {
        outfile << vStr[i] << " ";
    }
    //printf("%s", "aaa\n");
    return 0;
}