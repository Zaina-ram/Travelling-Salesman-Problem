#include "tsp.h"
#include <iostream>
#include <unordered_set>
#include <set>
#include <string.h>
#include "util.cpp"

using namespace std;

vector<tuple<int, int>> findMST(vector<vector<int>> distanceMat, int n)
{
    vector<tuple<int, int>> mst = vector<tuple<int, int>>();
    unordered_set<int> verticesNotInTree;
    for (int i = 1; i < n; i++){
        verticesNotInTree.insert(i);
    }
    vector<int> queue(n, 0);
    vector<int> parent(n, 0);

    // setup queue of priority (lower is first)
    for (int i = 1; i < n; i++)
    {
        queue[i] = distanceMat[0][i];
        parent[i] = 0;
    }

    // do i-1 times
    //   find lowest queue vertex w
    //   add parent-w to MST
    //   update queue
    for (int i = 1; i < n; i++)
    {
        int minqueue = 100000000; //>= 2sqrt(2)*10^6 (max possible distance)
        int minVertexParent;
        int minVertex;

        //  find lowest queue vertex w
        for (int w: verticesNotInTree)
        {
            if (queue[w] < minqueue)
            {
                minqueue = queue[w];
                minVertexParent = parent[w];
                minVertex = w;
            }
        }

        // add parent-v to MST
        // add v to set
        mst.push_back(make_tuple(minVertexParent, minVertex));
        verticesNotInTree.erase(minVertex);

        //  update queue
        for (int v: verticesNotInTree)
        {
            if (queue[v] > distanceMat[v][minVertex])
            {
                queue[v] = distanceMat[v][minVertex];
                parent[v] = minVertex;
            }
        }
    }
    return mst;
}

vector<int> findOddVertices(vector<tuple<int, int>> mst)
{
    int n = mst.size() + 1;
    int *vertexDegrees = new int[n];
    memset(vertexDegrees, 0, sizeof(int) * n);

    vector<int> oddVertices = vector<int>();
    for (int i = 0; i < mst.size(); i++)
    {

        tuple<int, int> tup = mst[i];
        vertexDegrees[get<0>(tup)] += 1;
        vertexDegrees[get<1>(tup)] += 1;
    }


    for (int i = 0; i < n; i++)
    {
        if (vertexDegrees[i] % 2 != 0)
        {
            oddVertices.push_back(i);
        }
    }

    return oddVertices;
}

/**
 * Finds all of the edges between vertices with odd degrees. The vertex indices of the edges are remapped
 * to [0, oddVertices.size()] such that the a vertex i in an edge tuple actually maps to vertex
 * oddVertices.at(i) in the original graph.
 * @return A list of edges and their costs.
*/
vector<tuple<int, int, int>> findEdges(vector<int> oddVertices, vector<vector<int>> distanceMat)
{
    vector<tuple<int, int, int>> oddEdges = {};
    for (int i = 0; i < oddVertices.size(); i++)
    {
        for (int j = i + 1; j < oddVertices.size(); j++)
        {
            size_t oddI = oddVertices.at(i);
            size_t oddJ = oddVertices.at(j);
            oddEdges.push_back(make_tuple(i, j, distanceMat[oddI][oddJ]));
        }
    }
    return oddEdges;
}

// input:
// points -> each point stored as a tuple containing coordinates x, y
// n -> number of points (tuples in points array)
vector<vector<int>> calculateDistances(vector<tuple<double, double>> points, int n)
{
    vector<vector<int>> distanceMat = vector<vector<int>>(n, vector<int>(n, 0));
    for (int i = 0; i < n; i++)
    {
        // cout << "outer iteration:" << i;
        // distanceMat[i][i] = 0;
        double x_i = get<0>(points[i]);
        double y_i = get<1>(points[i]);
        for (int j = i + 1; j < n; j++)
        {
            double x_j = get<0>(points[j]);
            double y_j = get<1>(points[j]);
            double x = x_j - x_i;
            double y = y_i - y_j;
            int dist = sqrt(pow(x, 2) + pow(y, 2)); // implicit conversion to int
            distanceMat[i][j] = dist;
            distanceMat[j][i] = dist;
        }
    }

    return distanceMat;
}
