#include <assert.h>
#include <iostream>
#include <stdarg.h>
#include <functional>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <utility>
#include <iostream>
#include <fstream>
#include <string>
#include <stack>

using namespace std;

class MinCostToBuyStampNaive
{
private:
    static int Helper(const vector<int> & stamp, int cur, int curAmount, int goal)
    {
        if (cur >= stamp.size())
            return (curAmount >= goal) ? curAmount : INT_MAX;
        if (curAmount >= goal) {
            return curAmount;
        }

        int cost = stamp[cur];
        int maxStamps = ceil((float)(goal - curAmount) / (float)cost);
        int minCostAboveGoal = INT_MAX;

        for (int i = 0; i <= maxStamps; i++) {
            int amount = Helper(stamp, cur + 1, curAmount + stamp[cur] * i, goal);
            minCostAboveGoal = min(amount, minCostAboveGoal);
        }

        return minCostAboveGoal;
    }
public:
    static float Do(const vector<float> & stamps, float goal)
    {
        vector<int> intStamps(stamps.size());
        for (size_t i = 0; i < intStamps.size(); i++) {
            intStamps[i] = (int)(stamps[i] * 100);
        }
        int intAmount = Helper(intStamps, 0, 0, static_cast<int>(goal * 100));
        return ((float)intAmount) / 100.f;
    }
};

class MinCostToBuyStampDPTopDown
{
private:
    static int Recursive(const vector<int> & stamps, int amount, float goal)
    {
        if (amount <= 0)
            return 0;

        int minAboveX = INT_MAX;
        for (auto i : stamps) {
            minAboveX = min(Recursive(stamps, amount - i, goal) + i, minAboveX);
        }

        return minAboveX;
    }
public:
    static float Do(const vector<float> & stamps, float goal)
    {
        vector<int> intStamps(stamps.size());
        for (size_t i = 0; i < intStamps.size(); i++) {
            intStamps[i] = (int)(stamps[i] * 100);
        }

        int minAboveX = Recursive(intStamps, (int)(goal * 100), goal);
        return ((float)minAboveX) / 100.f;
    }
};

class MinCostToBuyStampDPBottomUp
{
public:
    static float Do(const vector<float> & stamps, float goal)
    {
        vector<int> intStamps(stamps.size());
        for (size_t i = 0; i < intStamps.size(); i++) {
            intStamps[i] = (int)(stamps[i] * 100);
        }

        vector<int> minCostAboveGoal((goal * 100) + 1, INT_MAX);
        minCostAboveGoal[0] = 0;

        for (int i = 0; i < minCostAboveGoal.size(); i++) {
            for (int j = 0; j < intStamps.size(); j++) {
                if (i < intStamps[j]) {
                    minCostAboveGoal[i] = min(intStamps[j], minCostAboveGoal[i]);
                }
                else
                    minCostAboveGoal[i] = min(minCostAboveGoal[i - intStamps[j]] + intStamps[j], minCostAboveGoal[i]);
            }
        }

        return ((float)minCostAboveGoal.back()) / 100.f;
    }
};

/*
main output_iterator
3.01
3.01
3.01
*/

int main()
{
    vector<float> stamps{ 0.77f, 0.99f, 0.49f };
    float goal = 3.0;
    // Worst solution. O(2^N)
    cout << MinCostToBuyStampNaive::Do(stamps, goal) << endl;

	// Top Down DP solution O(N)
    cout << MinCostToBuyStampDPTopDown::Do(stamps, goal) << endl;
	
    // Bottom Up DP solution O(N)
    cout << MinCostToBuyStampDPBottomUp::Do(stamps, goal) << endl;

    return 0;
}
