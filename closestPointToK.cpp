/*
Problem: Find K closest points to reference point. # of points for input is millions.
*/

#include <iostream>
#include <queue>
#include <vector>

using namespace std;

struct Point
{
    Point(float _x, float _y) : x(_x), y(_y) {}
    float x, y;
};

class Compare
{
    Point ref;
public:
    Compare(Point _ref = Point(0.0, 0.0)) : ref(_ref) {}
    bool operator () (const Point & l, const Point & r) {
        return (l.x - ref.x) * (l.x - ref.x) + (l.y - ref.y) * (l.y - ref.y) < (r.x - ref.x) * (r.x - ref.x) - (r.y - ref.y) * (r.y - ref.y);
    }
};

vector<Point> FindClosest(const vector<Point> & points, Point ref, int k)
{
    priority_queue<Point, std::vector<Point>, Compare> heap(Point(0, 11));

    for (auto it = points.begin(); it != points.end(); it++) {
        heap.push(*it);
        if (heap.size() > k) heap.pop();
    }

    vector<Point> result;
    while (!heap.empty()) {
        result.push_back(heap.top());
        heap.pop();
    }
    return result;
}

int main()
{
    vector<Point> points = { Point(1, 1), Point(4, 1), Point(0.5, 11), Point(7, 1), Point(-1, 1), Point(-2, 1), Point(-11, 1) };
    vector<Point> res = FindClosest(points, Point(3.0, 4.0), 3);

    for (auto it = res.begin(); it != res.end(); it++) {
        cout << it->x << " : " << it->y << endl;
    }

    return 0;
}

