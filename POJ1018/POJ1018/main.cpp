
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <cstring>

using namespace std;

#define MAX_PRICE 65536
#define MAX_DEVICE 100
#define MAX_MANUFACTURER 100

typedef struct s {
    int bw;
    int pr;
} dev;

int main()
{
    int N;
    cin >> N;
    dev a[MAX_DEVICE+1][MAX_MANUFACTURER+1];
    int b[MAX_DEVICE*MAX_MANUFACTURER+1];
    int d[MAX_DEVICE+1];
    while(N--)
    {
        memset(a, 0, sizeof(a));
        memset(b, 0, sizeof(b));
        memset(d, 0, sizeof(d));
        int m;
        cin >> m;
        int bsize = 0;
        for (int i = 0; i < m; i++)
        {
            cin >> d[i];
            for (int j = 0; j < d[i]; j++)
            {
                cin >> a[i][j].bw >> a[i][j].pr;
                b[bsize++] = a[i][j].bw;
            }
        }
        sort(b, b+bsize);
        float maxBP = 0;
        for (int k = 0; k < bsize; k++)
        {
            if (k > 0 && b[k] == b[k-1])
            {
                continue;
            }
            int curBw = b[k];
            int prSum = 0;
            bool bwValid = true;
            for (int i = 0; i < m; i++)
            {
                int minPr = MAX_PRICE;
                for (int j = 0; j < d[i]; j++)
                {
                    if (a[i][j].bw >= curBw && a[i][j].pr < minPr)
                    {
                        minPr = a[i][j].pr;
                    }
                }
                if (minPr == MAX_PRICE)
                {
                    bwValid = false;
                    break;
                }
                prSum += minPr;
            }
            if (bwValid)
            {
                float curMax = ((float)curBw)/prSum;
                maxBP = curMax > maxBP ? curMax : maxBP;
            }
        }
        printf("%.3f\n", maxBP);
    }
    return 0;
}