
#include <iostream>
#include <cstdio>

int max(int x, int y)
{
    return x>y?x:y;
}

using namespace std;

int main()
{
    int max(int, int);
    int (*p)(int, int) = &max;
    int a=1,b=2,c=3,d;
    d=(*p)((*p)(a,b),c);
    printf("%d\n",d);
    return 0;
}