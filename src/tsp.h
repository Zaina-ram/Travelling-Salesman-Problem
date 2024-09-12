#include <vector>
#include <tuple>
#include <cmath>
using namespace std;


vector<vector<int>> calculateDistances(vector<tuple<double, double>> points, int n);
vector<tuple<int, int>> findMST(vector<vector<int>> distanceMat, int n);
vector<int> findOddVertices(vector<tuple<int, int>> mst);
vector<tuple<int, int, int>> findEdges(vector<int> oddVertices, vector<vector<int>> distanceMat);