#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <bits/stdc++.h>
#include <string>
#include <set>
#include <algorithm>

using namespace std;

class Point
{
public:
    float x, y;
    int clusterId;
    int id;
    vector<int> clusters;
};

int n_points = 0;
float eps = 0.3;
float epArea = 0.1;
int minPts = 5;
// Point points[150000];
vector<Point> points;
const int UNDEFINED = 0;
float max_Ox = -10000;
float min_Ox = 10000;
float max_Oy = -10000;
float min_Oy = 10000;
int num_slice_0x = 0;
int num_slice_Oy = 0;
int n_slices = 0;
const int MAX_SLICE = 1000;
vector<Point> partitions[MAX_SLICE];
vector<int> root;

// Disjoint set
// find root of node and update
int findAndUpdateRootNode(int node){
    if( root[node] == node )  return node;
    return root[node] = findAndUpdateRootNode(root[node]);
}

// find root of node and not update
int bruteFind(int node){
    if( root[node] == node )    return node;
    return bruteFind(root[node]);
}


void readPoints()
{
    FILE *f;
    f = fopen("../data/Circle.txt", "r");
    char point[255];
    while (fgets(point, 255, f))
    {
        char *p = strtok(point, " ");
        Point p_;
        p_.x = strtof(p, NULL);
        p = strtok(NULL, " ");
        p_.y = strtof(p, NULL);
        p = strtok(NULL, " ");
        max_Ox = max(max_Ox, p_.x);
        min_Ox = min(min_Ox, p_.x);
        max_Oy = max(max_Oy, p_.y);
        min_Oy = min(min_Oy, p_.y);
        p_.clusterId = UNDEFINED;
        p_.id = n_points;
        points.push_back(p_);
        n_points++;
    };
    fclose(f);
};

int calSlice()
{
    num_slice_0x = int((max_Ox - min_Ox) / (3 * eps)) + 1;
    num_slice_Oy = int((max_Oy - min_Oy) / (3 * eps)) + 1;
    return num_slice_0x * num_slice_Oy;
}

void pointsToFile()
{
    ofstream f;
    f.open("../data/merged.txt");
    for (int p = 0; p < n_points; p++)
    {
        string px(to_string(points[p].x));
        string py(to_string(points[p].y));
        string pl(to_string(points[p].clusterId));
        string pstr = px + " " + py + " " + pl + "\n";
        f << pstr;
    }
    f.close();
}

void partitionToFile(vector<Point> p, int idPartittion)
{
    ofstream f;
    string filename = "../data/partitions/" + to_string(idPartittion) + ".txt";
    f.open(filename);
    for (int i = 0; i < p.size(); i++)
    {
        string px(to_string(points[p[i].id].x));
        string py(to_string(points[p[i].id].y));
        string pl(to_string(points[p[i].id].clusterId));
        string pstr = px + " " + py + " " + pl + " " + "\n";

        f << pstr;
    }
    f.close();
}

float dist2points(Point p1, Point p2)
{
    return sqrt(pow((p2.x - p1.x), 2) + pow((p2.y - p1.y), 2));
}


set<int> findNeighbors(Point p, int idP)
{
    set<int> neighbors;
    for (int q = 0; q < partitions[idP].size(); q++)
    {
        if (dist2points(p, partitions[idP][q]) <= eps)
        {
            neighbors.insert(q);
        }
    }
    return neighbors;
}

void updateClustersPoint(int idPartition){
    for(int i=0;i<partitions[idPartition].size();i++){
        points[partitions[idPartition][i].id].clusters.push_back(partitions[idPartition][i].clusterId);
    }
}

void updateClusterIdPoints(){
    int ru,rv;
    for(int i=0;i<n_points;i++){
        sort(points[i].clusters.begin(),points[i].clusters.end());
        ru = findAndUpdateRootNode(points[i].clusters[0]);
        for(int j=1;j<points[i].clusters.size();j++){
            rv = findAndUpdateRootNode(points[i].clusters[j]);
            if(ru != rv){
                root[rv] = ru;
            }
        }
    }
    for(int i=0;i<n_points;i++){
        points[i].clusterId = root[points[i].clusters[0]];
    }
}

int c = 0;

int dbscan(int idP)
{
    if (partitions[idP].size() == 0)
        return 0;
    for (int p = 0; p < partitions[idP].size(); p++)
    {
        if (partitions[idP][p].clusterId != UNDEFINED)
            continue;
        set<int> neighbors = findNeighbors(partitions[idP][p], idP);
        if (neighbors.size() < minPts)
        {
            partitions[idP][p].clusterId = -1;
            continue;
        };
        c = c + 1;
        root.push_back(c);
        partitions[idP][p].clusterId = c;
        set<int> S = neighbors;
        while (S.size() > 0)
        {
            set<int>::iterator i = S.begin();
            S.erase(i);
            if (partitions[idP][*i].clusterId == -1)
            {
                partitions[idP][*i].clusterId = c;
            }
            if (partitions[idP][*i].clusterId != UNDEFINED)
                continue;
            partitions[idP][*i].clusterId = c;

            set<int> N = findNeighbors(partitions[idP][*i], idP);
            if (N.size() >= minPts)
            {
                S.insert(N.begin(), N.end());
            }
        }
    }
    updateClustersPoint(idP);
    return c;
};

void partitionPoints()
{
    int sX, sY, slice;
    float area_X_right, area_Y_top, area_Y_down, area_X_left;
    float area_eps = eps;
    for (int i = 0; i < n_points; i++)
    {
        sX = int(floor((points[i].x - min_Ox) / (3 * eps)));
        sY = int(floor((points[i].y - min_Oy) / (3 * eps)));
        area_X_right = min_Ox + (sX + 1) * 3 * eps;
        area_Y_top = min_Oy + (sY + 1) * 3 * eps;
        area_X_left = min_Ox + sX  * 3 * eps;
        area_Y_down = min_Oy + sY  * 3 * eps;

        slice = num_slice_0x * sY + sX;

        Point pointRepl = points[i];

        if (pointRepl.x > area_X_right - eps && pointRepl.x <= area_X_right)
        {
            partitions[slice + 1].push_back(pointRepl);
            if (pointRepl.y > area_Y_top - eps && pointRepl.y <= area_Y_top)
            {
                partitions[slice + num_slice_0x + 1].push_back(pointRepl);
                partitions[slice + num_slice_0x].push_back(pointRepl);
            }
            else if (pointRepl.y >= area_Y_down && pointRepl.y < area_Y_down + eps)
            {
                if (slice - num_slice_0x + 1 >= 0)
                {
                    partitions[slice - num_slice_0x + 1].push_back(pointRepl);
                    if (slice - num_slice_0x >= 0)
                    {
                        partitions[slice - num_slice_0x].push_back(pointRepl);
                    }
                }
            }
        }
        else if (pointRepl.x >= area_X_left && pointRepl.x < area_X_left + eps)
        {
            if (slice - 1 >= 0)
                partitions[slice - 1].push_back(pointRepl);
            if (pointRepl.y >= area_Y_top - eps && pointRepl.y < area_Y_top)
            {
                partitions[slice + num_slice_0x - 1].push_back(pointRepl);
                partitions[slice + num_slice_0x].push_back(pointRepl);
            }
            else if (pointRepl.y >= area_Y_down && pointRepl.y < area_Y_down + eps)
            {
                if (slice - num_slice_0x - 1 >= 0)
                {
                    partitions[slice - num_slice_0x - 1].push_back(pointRepl);
                    if (slice - num_slice_0x >= 0)
                    {
                        partitions[slice - num_slice_0x].push_back(pointRepl);
                    }
                }
            }
        };

        if (pointRepl.y > area_Y_top - eps && pointRepl.y <= area_Y_top)
        {
            partitions[slice + num_slice_0x].push_back(pointRepl);
        }
        else if (pointRepl.y >= area_Y_down && pointRepl.y < area_Y_down + eps)
        {
            if (slice - num_slice_0x >= 0)
                partitions[slice - num_slice_0x].push_back(pointRepl);
        };

        partitions[slice].push_back(pointRepl);
    }
}

int main()
{
    readPoints();
    n_slices = calSlice();
    cout << "max ox " << max_Ox << endl;
    cout << "min ox " << min_Oy << endl;
    cout << "max oy " << max_Oy << endl;
    cout << "min oy " << min_Oy << endl;
    cout << "slice ox: " << num_slice_0x << endl;
    cout << "slice oy: " << num_slice_Oy << endl;
    cout << "num slices: " << n_slices << endl;

    root.push_back(0);
    partitionPoints();
    int totalSize = 0;
    for (int i = 0; i <= n_slices; i++)
    {
        dbscan(i);
        totalSize += partitions[i].size();
    }
    updateClusterIdPoints();

    cout << "total size: " << totalSize << endl;
    cout << " n points: " << n_points << endl;
    cout << "partition 0 : " << partitions[0].size() << endl;
    
    pointsToFile();
    return 0;
}