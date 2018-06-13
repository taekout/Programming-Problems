#include <iostream>
#include <vector>
using namespace std;

void DFSWaysOfConnectionHelper(int minDepth, int maxDepth, int curDepth, int curNode,
                              vector<bool> & visited, const vector<vector<int>> & jump,
                              vector<int> & DPWays)
{
    if(minDepth <= curDepth && curDepth <= maxDepth) {
        DPWays[curDepth] ++;
        if(curDepth == maxDepth)
            return;
    }

    visited[curNode] = true;
    for(int i = 1 ; i < 10 ; i++) {
        if(visited[i] == false && (jump[curNode][i] == 0 || visited[jump[curNode][i]] == true)) {
            DFSWaysOfConnectionHelper(minDepth, maxDepth, curDepth + 1, i, visited, jump, DPWays);
        }
    }
    visited[curNode] = false;
}

int DFSWaysOfConnection(int minDepth, int maxDepth, int curDepth,
                        int curNode, const vector<vector<int>> & jump)
{
    vector<bool> visited(10, false);
    vector<int> DPways(10, 0); // DP saves # of ways for each depth. Always DPways[0] = 0;
    DFSWaysOfConnectionHelper(minDepth, maxDepth, curDepth, curNode, visited, jump, DPways);
    
    int count = 0;
    for(int i = minDepth ; i <= maxDepth ; i++) {
        count += DPways[i];
    }
    return count;
}

int waysOfConnection(int min, int max)
{
    // what is between two spots. 0 means no node in-between.
    vector<vector<int>> jump(10);
    for(size_t i = 0 ; i < jump.size() ; i++) {
        jump[i].resize(10, 0);
    }
    
    jump[1][3] = jump[3][1] = 2;
    jump[4][6] = jump[6][4] = 5;
    jump[7][9] = jump[9][7] = 8;
    jump[1][7] = jump[7][1] = 4;
    jump[2][8] = jump[8][2] = 5;
    jump[3][9] = jump[9][3] = 6;
    jump[1][9] = jump[9][1] = jump[3][7] = jump[7][3] = 5;

    int cases = 0;
    cases += DFSWaysOfConnection(min, max, 1, 1, jump) * 4;
    cases += DFSWaysOfConnection(min, max, 1, 2, jump) * 4;
    cases += DFSWaysOfConnection(min, max, 1, 5, jump);

    return cases;
}

int main() {
    int minConnect = 5;
    int maxConnect = 7;
 
    cout << waysOfConnection(minConnect, maxConnect);
 
    return 0;
}