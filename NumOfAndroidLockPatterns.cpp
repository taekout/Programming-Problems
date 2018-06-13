#include <iostream>
#include <vector>
using namespace std;

int DFSWaysOfConnectionHelper(int depth, int curDepth, int curNode,
vector<bool> & visited, const vector<vector<int>> & jump)
{
    if(curDepth == depth) {
        return 1;
    }
    
    visited[curNode] = true;
    cout << "visiting " << curNode << endl;
    int count = 0;
    for(int i = 1 ; i < 10 ; i++) {
        if(visited[i] == false && (jump[curNode][i] == 0 && visited[jump[curNode][i]] == true)) {
            count += DFSWaysOfConnectionHelper(depth, curDepth + 1, i, visited, jump);
        }
    }
    visited[curNode] = false;
    
    return count;
}

int DFSWaysOfConnection(int depth, int cur, const vector<vector<int>> & jump)
{
    vector<bool> visited(9, false);
    
    return DFSWaysOfConnectionHelper(depth, 1, cur, visited, jump);
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
    jump[1][9] = jump[9][1] = 5;
    jump[3][7] = jump[7][3] = 5;

    int cases = 0;
    for(int i = min ; i <= max ; i++) {
        for(int j = 1 ; j < 10 ; j++) {
            cases += DFSWaysOfConnection(i, j, jump);
        }
    }
    
    return cases;
    // triangle case * 4, rectangle case * 4, star case * 1.
    //int nCases = DFSWaysOfConnection(min, max, visited, Point(0, 0)) * 4;
    //nCases += DFSWaysOfConnection(min, max, visited, Point(1, 0)) * 4;
    //nCases += DFSWaysOfConnection(min, max, visited, Point(1, 1));
}

int main() {
	int minConnect = 4;
    int maxConnect = 6;
 
    cout << waysOfConnection(minConnect, maxConnect);
 
    return 0;
}