using namespace std;
#include <tuple>
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <unordered_set>
#include <map>
#include <string.h>
#include <algorithm>
#include <list>

#define TEST
#include "tsp.h"
#include "util.cpp"
//#include "./Minimum-Cost-Perfect-Matching-master/Example.cpp"
#ifdef TEST
    #include "./PM_2/PerfectMatching.h"
#else
    #include "PerfectMatching.h"
#endif

vector<list<int>> MSTintoVectorList(vector<tuple<int, int>>& edgesAsTup, int n);
void CalculateEulerianTourStep(vector<list<int>>& adjList, vector<int>& EulerianTour, int startingvertex);
inline int lerpedRand(int LO, int HI);
void attempt2Opt(int TSPTour[], int n, int& origCost);
void attempt3Opt(int TSPTour[], int n, int& origCost);
int calcCost(vector<int>& TSPtour);
void reverseReorder(int x1, int y1, int z1, int n, bool replace, bool flipXY, bool flipYZ);
void reverseCopy(int* dest, int* source, int n);


vector<vector<int>> DistanceMatrix;

int* tours[2];
int currentTour = 0;

int main() {
    //small testset
    //int n=10;
    //vector<tuple<double, double>> points = { make_tuple( 95.0129,61.5432 ), make_tuple( 23.1139,79.1937), make_tuple( 60.6843,92.1813), make_tuple( 48.5982,73.8207), make_tuple(89.1299,17.6266),make_tuple(76.2097,40.5706),make_tuple(45.6468,93.5470),make_tuple(1.8504,91.6904),make_tuple(82.1407,41.0270),make_tuple(44.4703,89.3650) };
    
#ifdef TEST    
    srand(745);
    int n=1000;
    int HI = 1000000;
    int LO = -1000000;
    vector<tuple<double, double>> points;
    for(int i = 0; i<n; i++){
        float r1 = LO + (double) (rand()) / RAND_MAX * (HI-LO);
        float r2 = LO + (double) (rand()) / RAND_MAX * (HI-LO);
        points.push_back(make_tuple(r1,r2));
    }
#else
    //large random testset
    int n;
    cin >> n;
    vector<tuple<double, double>> points;
    
    for(int i=0;i<n;i++) {
        double in_x;
        double in_y;
        cin >> in_x;
        cin >> in_y;
        tuple<double,double> point = make_tuple(in_x,in_y);
        points.push_back(point);
    }
#endif


    //behold THE SYNTAX
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point fullstart = std::chrono::high_resolution_clock::now();

    DistanceMatrix = calculateDistances(points,  n);
    
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = (stop - start);
    #ifdef TEST
        cout << "Duration of Distance Matrix Calculation (in ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << endl;
    #endif

    start = std::chrono::high_resolution_clock::now();
    vector<tuple<int, int>> mst = findMST(DistanceMatrix,n);
    stop = std::chrono::high_resolution_clock::now();
    duration = (stop - start);

    #ifdef TEST
        cout << "Duration of MST Calculation (in ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << endl;
    #endif

    start = std::chrono::high_resolution_clock::now();
    vector<int> oddVer = findOddVertices(mst);
    vector<tuple<int, int,int>> oddEdges = findEdges(oddVer, DistanceMatrix);

    stop = std::chrono::high_resolution_clock::now();
    duration = (stop - start);
    #ifdef TEST
        cout << "Duration of Calculating odd edges (in ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << endl;
        //cout << "Number of Odd Vertices:" << oddVer.size() << endl;
    #endif

    start = std::chrono::high_resolution_clock::now();
    //----------------------------Perfect matching----------------------------
        PerfectMatching matcher = PerfectMatching(oddVer.size(), oddEdges.size());
        matcher.options.verbose = false;
        //Add all the edges of the odd vertices to the matcher
        for(int i = 0; i < oddEdges.size(); i++)
        {
            tuple<int, int, int> edge = oddEdges.at(i);
            matcher.AddEdge(get<0>(edge), get<1>(edge), get<2>(edge));
        }
        matcher.Solve();
        for(int i = 0; i < oddEdges.size(); i++)
        {
            if(matcher.GetSolution(i) == 1)
            {
                int vert1 = oddVer.at(get<0>(oddEdges.at(i)));
                int vert2 = oddVer.at(get<1>(oddEdges.at(i)));
                mst.push_back({vert1, vert2});
            }
        }
    //------------------------------------------------------------------------
    stop = std::chrono::high_resolution_clock::now();
    duration = (stop - start);
    #ifdef TEST
        cout << "Duration of Calculating Minimum Cost Perfect Matching (in ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << endl;
    #endif
    
    start = std::chrono::high_resolution_clock::now();
    
    //----------------------------Eulerian circuit attempt 3 (did work)-------
    
        vector<int> EulerianTour;
        vector<list<int>> adjList = MSTintoVectorList(mst, n);
        CalculateEulerianTourStep(adjList, EulerianTour, 0);

    //------------------------------------------------------------------------


    stop = std::chrono::high_resolution_clock::now();
    duration = (stop - start);
    #ifdef TEST
        cout << "Duration of Calculating Eulerian circuit (in ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << endl;
        //cout << "Eulerian Tour: " << EulerianTour << endl;
    #endif

    start = std::chrono::high_resolution_clock::now();
    //----------------------------TSP Tour----------------------------
        vector<int> TSPtour;
        for(int vert: EulerianTour)
        {
            if(count(TSPtour.begin(), TSPtour.end(), vert) == 0)
                TSPtour.push_back(vert);
        }

    //--------------------------------------------------------------
    stop = std::chrono::high_resolution_clock::now();
    duration = (stop - start);
    int cost = calcCost(TSPtour);
    
    #ifdef TEST
        cout << "Duration of Calculating TSP (in ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << endl;
        cout << "Cost: " << cost << endl;
    #endif
    
    int beginningcost = cost;
    //method for testing cost improvement

    tours[0] = new int[n];
    tours[1] = new int[n];
    copy(TSPtour.begin(), TSPtour.end(), tours[0]);

    stop = std::chrono::high_resolution_clock::now();
    int count = 0;
    while(std::chrono::duration_cast<std::chrono::milliseconds>(stop - fullstart).count() < 500)
    {
        for(int i = 0; i < 10000; i++)
            attempt2Opt(tours[currentTour], n, cost);
        count += 10000;
        stop = std::chrono::high_resolution_clock::now();
        #ifdef TEST
            //cout << "Cost: " << cost << endl;
            //cout << "Count: " << count << endl;
            //cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - fullstart).count() << endl;
        #endif
    }

    while(std::chrono::duration_cast<std::chrono::milliseconds>(stop - fullstart).count() < 1995)
    {
        for(int i = 0; i < 200; i++)
            attempt2Opt(tours[currentTour], n, cost);
        for(int i = 0; i < 800; i++)
            attempt3Opt(tours[currentTour], n, cost);
        count += 1000;
        stop = std::chrono::high_resolution_clock::now();
        #ifdef TEST
            //cout << "Cost: " << cost << endl;
            //cout << "Count: " << count << endl;
            //cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - fullstart).count() << endl;
        #endif
    }
    

    //timed loop


    #ifdef TEST
        cout << "Optimised savings: " << beginningcost - cost << endl;
        cout << count << endl;
    #else
        for(int i = 0; i < n; i++)
            cout << tours[currentTour][i] << endl;
    #endif

    stop = std::chrono::high_resolution_clock::now();
    duration = (stop - fullstart);
    #ifdef TEST
        cout << "Total duration (in ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << endl;
    #endif

    return 0;
}

//one iteration of 2opt

vector<list<int>> MSTintoVectorList(vector<tuple<int, int>>& edgesAsTup, int n) {
   vector<list<int>> adjList(n);

    for(int i = 0; i < edgesAsTup.size(); i++) {

        int v1 = get<0>(edgesAsTup.at(i));
        int v2 = get<1>(edgesAsTup.at(i));
        adjList[v1].push_back(v2);
        adjList[v2].push_back(v1);       
    }

    return adjList;
}

/**
* EXPLANATION: Okay how the hell does this recursive wizardry work?
* So basically, we imagine a little worm happily eating along through the graph one edge at a time.
* It only stops when there are no more edges to eat. This occurs only if the worm has reached its butt (its starting position).
* But as of this moment we need not necessarily be done (indeed, we have just made a cycle, which is where we stopped before).
* Instead, because of the recursive calls, we now go back to the last intersection (this is the while size > 0)
* As we are going back, we imagine the worm to leave a slimy (numbered) trail for us to follow, the path.
* Or perhaps he hooks a string into his butt and pulls it along with its mouth, dropping it to eat. At any rate:
* At the aforementioned intersection, the worm now gets another chance to eat, finishing again only when he reaches the same intersection again with no other options.
* Now he again retreads his steps, leaving more path for us to follow. Because the path leads from his head backwards as he retreats,
* it connects to where our path left off before.
* In this fashion, the worm keeps pulling backwards, taking every opportunity to eat away all he can.
* In the end, he shrinks back to his butt, having pulled the string once through the entire graph, never using an edge twice.
* @param adjList The vector of linked lists adjacency list representation of the graph (with multiedges)
* @param EulerianTour The tour, filled out on the fly
* @param startingvertex The vertex to initiate the recursive function from
* @return Nothing (EulerianTour has been modified to be correct)
*/void CalculateEulerianTourStep(vector<list<int>>& adjList, vector<int>& EulerianTour, int startingvertex){
    int endingvertex;
    while (adjList[startingvertex].size()>0)
    {
        endingvertex = adjList[startingvertex].front();
        adjList[startingvertex].pop_front();
        auto pos = find(adjList[endingvertex].begin(), adjList[endingvertex].end(), startingvertex);
        adjList[endingvertex].erase(pos);
        CalculateEulerianTourStep(adjList, EulerianTour, endingvertex);
    }
    EulerianTour.push_back(startingvertex);
}

void attempt2Opt(int TSPTour[], int n, int& origCost)
{
    if(n <= 1) return;
    int a = lerpedRand(0, n - 2);
    int b = lerpedRand(0, n - 2);
    while(a == b)
        b = lerpedRand(0, n - 2);

    int v1 = TSPTour[a];
    int v2 = TSPTour[a + 1];
    int v3 = TSPTour[b];
    int v4 = TSPTour[b + 1];

    int delta = - DistanceMatrix[v1][v2] - DistanceMatrix[v3][v4] + DistanceMatrix[v1][v3] + DistanceMatrix[v2][v4];
    if(delta < 0)
    {
        if(a > b)
        {
            int tmp = a;
            a = b;
            b = tmp;
        }
        
        reverse(TSPTour + a + 1, TSPTour + b + 1);
        origCost += delta;
    }
}

void attempt3Opt(int TSPTour[], int n, int& origCost)
{
    if(n <= 2) return;
    int a, b, c;
    if(n <= 100)
    {
        a = lerpedRand(0, n - 2);
        b = lerpedRand(0, n - 2);
        c = lerpedRand(0, n - 2);
        while(a == b || b == c || c == a){
            a = lerpedRand(0, n - 2);
            b = lerpedRand(0, n - 2);
            c = lerpedRand(0, n - 2);
        }
    }
    else
    {
        int randStep = max(n / 20, 1);
        a = lerpedRand(0, n - 2 * randStep - 2);
        b = a + lerpedRand(1, randStep);
        c = b + lerpedRand(1, randStep);
        while(a == b || b == c || c == a){
            a = lerpedRand(0, n - 2 * randStep - 2);
            b = a + lerpedRand(1, randStep);
            c = b + lerpedRand(1, randStep);
        }
    }
    int E[3] = { a, b, c };
    sort(E, E + 3);
    a = E[0];
    b = E[1];
    c = E[2];

    int x1 = TSPTour[a];
    int x2 = TSPTour[a + 1];
    int y1 = TSPTour[b];
    int y2 = TSPTour[b + 1];
    int z1 = TSPTour[c];
    int z2 = TSPTour[c + 1];

    //check all variants
    //denote XY YZ ZX the segments between, for example YZ between y2 and z1, we write ' for needing to reverse
    //v1: x1-y1 x2-z1 y2-z2: XY' YZ' ZX
    //v2: x1-z1 y2-x2 y1-z2: XY' YZ  ZX' 
    //v3: x1-y2 z1-y1 x2-z2: XY  YZ' ZX' 
    //v4: x1-y2 z1-x2 y1-z2: XY' YZ' ZX'
    //cout << "a:" << a << " b:" << b << " c:" << c << endl;
    //cout << "x1:" << x1 << " x2:" << x2 << " y1:" << y1  << " y2:" << y2 << " z1:" << z1 << " z2:" << z2 << endl;
    int costs [5] = {
        DistanceMatrix[x1][x2] + DistanceMatrix[y1][y2] + DistanceMatrix[z1][z2],
        DistanceMatrix[x1][y1] + DistanceMatrix[x2][z1] + DistanceMatrix[y2][z2],
        DistanceMatrix[x1][z1] + DistanceMatrix[y2][x2] + DistanceMatrix[y1][z2],
        DistanceMatrix[x1][y2] + DistanceMatrix[z1][y1] + DistanceMatrix[x2][z2],
        DistanceMatrix[x1][y2] + DistanceMatrix[z1][x2] + DistanceMatrix[y1][z2]
    };
    //find minimum cost
    int mincostvariant = 0;
    for (int i = 1; i < 5; i++)
    {   
        if (costs[i]<costs[mincostvariant])
        {
            mincostvariant = i;
            //cout << "foundsomethingbetter, variant: " << i << endl;
            //cout << "Costs: " << costs[0] << ", " << costs[1] << ", " << costs[2] << ", "<< costs[3] << ", "<< costs[4] << ", " << endl;
        }
    }

    //v1: x1-y1 x2-z1 y2-z2: XY' YZ' ZX
    //v2: x1-z1 y2-x2 y1-z2: XY' YZ  ZX' 
    //v3: x1-y2 z1-y1 x2-z2: XY  YZ' ZX' 
    //v4: x1-y2 z1-x2 y1-z2: XY' YZ' ZX' 
    //we rewrite these so that ZX is never reversed and always first
    //v1: ZX  XY' YZ'
    //v2: ZX  YZ' XY  
    //v3: ZX  YZ  XY'
    //v4: ZX  YZ  XY
    switch (mincostvariant) //XY' means reverse everything [x2,y1], both inclusive
    {
    case 0:
        return;
        break;
    case 1:
        reverseReorder(a, b, c, n, false, true, true);
        break;
    case 2:
        reverseReorder(a, b, c, n, true, false, true);
        break;

    case 3:
        reverseReorder(a, b, c, n, true, true, false);
        break;

    case 4:
        reverseReorder(a, b, c, n, true, false, false);
        break;


    default:
        break;
    }
    
    origCost += -costs[0] + costs[mincostvariant];
    return;
}

inline int lerpedRand(int LO, int HI)
{
    return LO + (double) (rand()) / RAND_MAX * (HI-LO);
}

int calcCost(vector<int>& TSPtour)
{
    int n = TSPtour.size();
    int dist = 0;
    for(int i = 0; i < n - 1; i++)
    {
        dist += DistanceMatrix[TSPtour[i]][TSPtour[i + 1]];
    }
    return dist + DistanceMatrix[TSPtour[n - 1]][TSPtour[0]];
}

void reverseReorder(int x1, int y1, int z1, int n, bool replace, bool flipXY, bool flipYZ)
{
    int other = 1 - currentTour;
    int n_zx = x1 + 1;
    int n_xy = y1 - x1;
    int n_yz = z1 - y1;
    int n_zx2 = n - z1 - 1;
    //printf("Replace: %d FlipXY: %d FlipYZ: %d\n", replace, flipXY, flipYZ);
    memcpy(tours[other], tours[currentTour], sizeof(int) * n_zx);
    if(replace)
    {
        if(flipYZ) reverseCopy(tours[other] + n_zx, tours[currentTour] + n_zx + n_xy, n_yz);
        else memcpy(tours[other] + n_zx, tours[currentTour] + n_zx + n_xy, sizeof(int) * n_yz);

        if(flipXY) reverseCopy(tours[other] + n_zx + n_yz, tours[currentTour] + n_zx, n_xy);
        else memcpy(tours[other] + n_zx + n_yz, tours[currentTour] + n_zx, sizeof(int) * n_xy);
        
        memcpy(tours[other] + n_zx + n_yz + n_xy, tours[currentTour] + n_zx + n_xy + n_yz, sizeof(int) * n_zx2);
    }
    else
    {
        if(flipYZ) reverseCopy(tours[other] + n_zx + n_xy, tours[currentTour] + n_zx + n_xy, n_yz);
        else memcpy(tours[other] + n_zx + n_xy, tours[currentTour] + n_zx + n_xy, sizeof(int) * n_yz);

        if(flipXY) reverseCopy(tours[other] + n_zx, tours[currentTour] + n_zx, n_xy);
        else memcpy(tours[other] + n_zx, tours[currentTour] + n_zx, sizeof(int) * n_xy);

        memcpy(tours[other] + n_zx + n_yz + n_xy, tours[currentTour] + n_zx + n_xy + n_yz, sizeof(int) * n_zx2);
    }

    currentTour = other;
}

void reverseCopy(int* dest, int* source, int n)
{
    source -= 1;
    for(int i = 0; i < n; i++)
        dest[i] = source[n - i];
}