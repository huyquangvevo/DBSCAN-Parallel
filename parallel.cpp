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
    num_slice_0x = int(round((max_Ox - min_Ox) / (3 * eps)));
    num_slice_Oy = int(round((max_Oy - min_Oy) / (3 * eps)));
    return num_slice_0x * num_slice_Oy;
}

void pointsToFile()
{
    ofstream f;
    f.open("./data/merged.txt");
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
        string px(to_string(points[p[i].id].x));
        string py(to_string(points[p[i].id].y));
        string pl(to_string(points[p[i].id].clusterId));
        // string plocal(to_string(p[i].isLocalRegion));
        // string pstr = px + " " + py + " " + pl + " " + plocal + "\n";
        string pstr = px + " " + py + " " + pl + " " + "\n";

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

int getNextSlice(int idP)
{
    return (idP % 4 == num_slice_0x - 1) ? idP : idP + 1;
}

set<int> findNeighbors(Point p, int idP)
{
    set<int> neighbors;
    for (int q = 0; q < partitions[idP].size(); q++)
    {
        if (dist2points(p, partitions[idP][q]) <= eps)
        {
            neighbors.insert(partitions[idP][q].id);
        }
    }
    return neighbors;
}


set<int> findNeighbors(Point p)
{
    set<int> neighbors;
    int cluster = 0;
    set<int> clusterNeighbors;
    for (int q = 0; q < points.size(); q++)
    {
        if (dist2points(p, points[q]) <= eps)
        {
            neighbors.insert(points[q].id);
            // cout << "Point cluster id: " << points[q].clusterId << endl;
            clusterNeighbors.insert(points[q].clusterId);
        }
    }
    // cout << "Cluster size: " << clusterNeighbors.size() << " : " << *clusterNeighbors.begin() << endl;
    return neighbors;
}

int c = 0;
int dbscan(int idP)
{
    // int c = 0;
    // cout << "partition : " << idP << " size: " << points.size() << endl;
    vector<Point> cell = partitions[idP];
    if (cell.size() == 0)
        return 0;
    for (int p = 0; p < cell.size(); p++)
    {
        if (points[cell[p].id].clusterId != UNDEFINED)
            continue;
        set<int> neighbors = findNeighbors(points[cell[p].id], idP);
        if (neighbors.size() < minPts)
        {
            points[cell[p].id].clusterId = -1;
            continue;
        };
        c = c + 1;
        points[cell[p].id].clusterId = c;
        set<int> S = neighbors;
        while (S.size() > 0)
        {
            set<int>::iterator i = S.begin();
            S.erase(i);
            if (points[*i].clusterId == -1)
            {
                points[*i].clusterId = c;
            }
            if (points[*i].clusterId != UNDEFINED)
                continue;
            points[*i].clusterId = c;

            set<int> N = findNeighbors(points[*i], idP);
            if (N.size() >= minPts)
            {
                S.insert(N.begin(), N.end());
            }
        }
    }
    cout << "partition : " << idP << " size: " << points.size() << " cluster: " << c << endl;
    return c;
};

void partitionPoints()
{
    int sX, sY, slice;
    float area_X_right, area_Y_top, area_Y_down, area_X_left;
    float area_eps = eps / 6;
    for (int i = 0; i < n_points; i++)
    {
        sX = int(floor((points[i].x - min_Ox) / (3 * eps)));
        sY = int(floor((points[i].y - min_Oy) / (3 * eps)));
        area_X_right = min_Ox + (sX + 1) * 3 * eps;
        area_Y_top = min_Oy + (sY + 1) * 3 * eps;
        area_X_left = min_Ox + sX * 3 * eps;
        area_Y_down = min_Oy + sY * 3 * eps;

        if ((points[i].x > area_X_right - area_eps && points[i].x < area_X_right) ||
            (points[i].y > area_Y_top - area_eps && points[i].y < area_Y_top) ||
            (points[i].x > area_X_left && points[i].x < area_X_left + area_eps) ||
            (points[i].y > area_Y_down && points[i].y < area_Y_down + area_eps))
            points[i].isLocalRegion = false;
        slice = num_slice_0x * sY + sX;
        partitions[slice].push_back(points[i]);
    }
}

int findMin(set<int> clusterSet)
{
    int min_element = 1000;
    set<int>::iterator it;
    cout << "find min... " << endl;
    for (it = clusterSet.begin(); it != clusterSet.end(); ++it)
    {
        cout << "find min item: " << *it<< endl;
        if (*it != -1)
            min_element = min(min_element, *it);
    }
    // cout << "min: " << min_element << endl;
    return (min_element == 1000) ? -1 : min_element;
}

set<int> getSliceNeighbors(int idP)
{
    set<int> slices;
    int rightSlice = (idP + 1 > num_slice_0x - 1) ? idP : idP + 1; //getNextSlice(idP);
    int leftSlice = (((idP - 1) % num_slice_0x == (num_slice_0x - 1)) || (idP - 1 < 0)) ? 0 : idP - 1;
    int downSlice = (idP - num_slice_Oy < 0) ? idP : idP - num_slice_Oy;
    int upSlice = (idP + num_slice_Oy > num_slice_Oy) ? idP : idP + num_slice_Oy;
    slices.insert(rightSlice);
    slices.insert(leftSlice);
    slices.insert(downSlice);
    slices.insert(upSlice);
    return slices;
}

set<int> getWindowsSlice(int index)
{
    set<int> windows;
    // if ((index + 1) % num_slice_0x == 0)
        // return windows;
    windows.insert(index);
    windows.insert((index + 1) >= n_slices ? index : index + 1);
    windows.insert((index + num_slice_0x) >= n_slices ? index : index + num_slice_0x);
    windows.insert((index + num_slice_0x + 1) >= n_slices ? index : index + num_slice_0x + 1);
    // set<int>::iterator it;
    // cout << "index slice : " << index << endl;
    // for (it = windows.begin(); it != windows.end(); ++it)
    // {
    //     cout << "window slice: " << *it << endl;
    // }
    return windows;
}

set<int> getAllClusterNeighbors(Point p, int idP)
{
    set<int> clusters;
    clusters.insert(p.clusterId);
    set<int> windows = getWindowsSlice(idP);
    set<int>::iterator it;
    vector<Point> pointsCell;
    // cout << "windows size all : " << windows.size() << endl;
    for (it = windows.begin(); it != windows.end(); ++it)
    {
        pointsCell = partitions[*it];
        // cout << "partitions size all : " << pointsCell.size() << endl;

        for (int i=0; i < pointsCell.size(); i++)
        {
            if (dist2points(p, pointsCell[i]) <= eps)
            {
                clusters.insert(points[pointsCell[i].id].clusterId);
            }
        }
    }
    // cout << "cluster size get all : " << clusters.size() << endl;
    return clusters;
}

void updateClusterId(set<int> clustersId, int idP)
{
    int clusterId_min = findMin(clustersId);
    // cout << "cluster id min: " << clusterId_min << endl;
    vector<Point> cell = partitions[idP];
    int clusterIdPoint;
    // cout << "updating..." << endl;
    set<int> windows = getWindowsSlice(idP);
    if(windows.size()==0)
        return;
    set<int>::iterator it;
    // cout << "windows size " << windows.size() << endl;
    // cout << "cluster size " << clustersId.size() << endl; 
    for (it = windows.begin(); it != windows.end(); ++it)
    {
        cell = partitions[*it];
        // cout << "cell size: " << cell.size() << endl;
        for (int q; q < cell.size(); q++)
        {
            set<int>::iterator itc;
            clusterIdPoint = points[cell[q].id].clusterId;
            for (itc = clustersId.begin(); itc != clustersId.end(); ++itc)
            {
                if (clusterIdPoint == *itc)
                    points[cell[q].id].clusterId = clusterId_min;
            }
        }
    }
    // cout << "end one updating." << endl;
}


void merge()
{
    for (int i = 0; i < n_slices; i++)
    {
        if((i+1)%num_slice_0x == 0)
            continue;
        vector<Point> pointsPartition = partitions[i];
        for (int j = 0; j < pointsPartition.size(); j++)
        {
            if (!points[pointsPartition[j].id].isLocalRegion)
            {
                set<int> clustersId = getAllClusterNeighbors(points[pointsPartition[j].id], i);
                updateClusterId(clustersId, i);
            }
        }
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
    // getWindowsSlice(13);
    // exit(0);
    partitionPoints();
    for (int i = 0; i < n_slices; i++)
    {
        dbscan(i);
        // partitionToFile(partitions[i], i);
    }
    merge();
    pointsToFile();
    // for (int i = 0; i < n_slices; i++)
    // {
    //     partitionToFile(partitions[i], i);
    // }
    return 0;
}