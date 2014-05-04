
/************************************************************************/
/* 枚举加贪心， 求前X个湖的各个时间段内所能钓到的鱼的数目，排序，然后
   算出钓鱼的实际时间段数目k，取前k个时间段钓的鱼的数目之和，就是最大数目
/************************************************************************/
#include <iostream>
#include <cstring>
#include <algorithm>

using namespace std;

typedef struct s {
    int lake;
    int itvlFish;
} myFish;

int cmp(myFish a, myFish b)
{
    if (a.itvlFish > b.itvlFish)
    {
        return 1;
    }
    else if (a.itvlFish == b.itvlFish && a.lake < b.lake)
    {
        return 1;
    }
    else
        return 0;
}

#define MAX_LAKE 25
int main()
{
    // fish[i][j] 第i个湖在第j个interval可以钓到的鱼
    int fish[MAX_LAKE][192+1];
    // interFish 存放各个interval下能钓到的鱼的数目
    myFish interFish[192+1];
    
    int n;
    bool isFirstCase = true;
    while(cin >> n && n != 0)
    {
        if (!isFirstCase)
        {
            cout << endl;
        }
        else
        {
            isFirstCase = false;
        }

        int interFishSize = 0;
        memset(fish, 0, sizeof(fish));
        memset(interFish, 0, sizeof(interFish));
        int h;
        cin >> h;
        int intervals = h*60/5;
        int f[MAX_LAKE], d[MAX_LAKE], t[MAX_LAKE-1], itvlToLake[MAX_LAKE-1];
        int useTime[MAX_LAKE];
        memset(useTime, 0, sizeof(useTime));
        useTime[0] = intervals;

        for (int i = 0; i < n; i++)
        {
            cin >> f[i];
        }
        for (int i = 0; i < n; i++)
        {
            cin >> d[i];
        }
        int itvl = 0;
        for (int i = 0; i < n-1; i++)
        {
            cin >> t[i];
            itvl += t[i];
            itvlToLake[i] = itvl;
        }
        
        int maxFish = 0;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < intervals; j++)
            {
                if (f[i] - d[i]*j > 0)
                {
                    fish[i][j] = f[i] - d[i]*j;
                    myFish m;
                    m.lake = i;
                    m.itvlFish = fish[i][j];
                    interFish[interFishSize++] = m;
                }
            }
            sort(interFish, interFish+interFishSize, cmp);
            int restItvls = intervals;
            if (i > 0)
            {
                restItvls = intervals - itvlToLake[i-1];
            }
            int sumFish = 0;
            
            int realFishCount = min(restItvls, interFishSize);
            for (int k = 0; k < realFishCount; k++)
            {
                sumFish += interFish[k].itvlFish;
            }
            //maxFish = max(maxFish, sumFish);
            if (sumFish > maxFish)
            {
                maxFish = sumFish;
                memset(useTime, 0, sizeof(useTime));
                for (int k = 0; k < realFishCount; k++)
                {
                    useTime[interFish[k].lake]++;
                }
                if (i > 0)
                {
                    useTime[0] += intervals - realFishCount - itvlToLake[i-1];
                }
                else
                {
                    useTime[0] += intervals - realFishCount;
                }
            }
        }
        for (int i = 0; i < n; i++)
        {
            if (i > 0)
            {
                cout << ", ";
            }
            cout << useTime[i]*5;
        }
        cout << " " << endl;
        cout << "Number of fish expected: " << maxFish << " " << endl;
    }
    return 0;
}