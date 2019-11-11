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
    bool isLocalRegion;
    bool isCorePoint;
    int id;
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

void readPoints()
{
    FILE *f;
    f = fopen("./data/Circle.txt", "r");
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
        p_.isLocalRegion = true;
        p_.isCorePoint = false;
        p_.id = n_points;
        points.push_back(p_);
        n_points++;
    };
    fclose(f);
};

int calSlice()
{
    num_slice_0x = int(round((max_Ox - min_Ox) / (3*eps)));
    num_slice_Oy = int(round((max_Oy - min_Oy) / (3*eps)));
    return num_slice_0x * num_slice_Oy;
}

void pointsToFile()
{
    ofstream f;
    f.open("./data/clustered2.txt");
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

void partitionToFile(vector<Point> p, int i)
{
    ofstream f;
    string filename = "./data/partitions/" + to_string(i) + ".txt";
    f.open(filename);
    for (int i = 0; i < p.size(); i++)
    {
        string px(to_string(p[i].x));
        string py(to_string(p[i].y));
        string pl(to_string(p[i].clusterId));
        string pstr = px + " " + py + " " + pl + "\n";
        f << pstr;
    }
    f.close();
}

float dist2points(Point p1, Point p2)
{
    return sqrt(pow((p2.x - p1.x), 2) + pow((p2.y - p1.y), 2));
}

bool comparePoint(Point p1, Point p2)
{
    return ((p1.x == p2.x) && (p1.y == p2.y) && (p1.clusterId == p2.clusterId));
}

set<int> rangeQuery(Point p, int id_P)
{
    set<int> neighbors;
    for (int q = 0; q < partitions[id_P].size(); q++)
    {
        if (dist2points(p, partitions[id_P][q]) <= eps)
        {
            neighbors.insert(points[q].id);
        }
    }
    return neighbors;
}

int dbscan(int id_P)
{
    int c = 0;
    // cout << "partition : " << id_P << " size: " << partitions[id_P].size() << endl;
    if (partitions[id_P].size() == 0)
        return 0;
    for (int p = 0; p < partitions[id_P].size(); p++)
    {
        if (partitions[id_P][p].clusterId != UNDEFINED)
            continue;
        set<int> neighbors = rangeQuery(partitions[id_P][p], id_P);
        if (neighbors.size() < minPts)
        {
            partitions[id_P][p].clusterId = -1;
            continue;
        };
        c = c + 1;
        partitions[id_P][p].clusterId = c;
        set<int> S = neighbors;
        while (S.size() > 0)
        {
            set<int>::iterator i = S.begin();
            S.erase(i);
            if (partitions[id_P][*i].clusterId == -1)
            {
                partitions[id_P][*i].clusterId = c;
            }
            if (partitions[id_P][*i].clusterId != UNDEFINED)
                continue;
            partitions[id_P][*i].clusterId = c;

            set<int> N = rangeQuery(partitions[id_P][*i], id_P);
            if (N.size() >= minPts)
            {
                S.insert(N.begin(), N.end());
            }
        }
    }
    cout << "partition : " << id_P << " size: " << partitions[id_P].size() << " cluster: " << c << endl;
    return c;
};

void partitionPoints()
{
    int sX, sY, slice;
    for (int i = 0; i < n_points; i++)
    {
        sX = int(floor((points[i].x - min_Ox) / (3*eps)));
        sY = int(floor((points[i].y - min_Oy) / (3*eps)));
        slice = num_slice_0x * sY + sX;
        partitions[slice].push_back(points[i]);
    }
}

void merge()
{
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
    partitionPoints();
    for (int i = 0; i < n_slices; i++)
    {
        dbscan(i);
        partitionToFile(partitions[i], i);
    }
    return 0;
}