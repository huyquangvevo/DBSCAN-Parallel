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
#include <mpi.h>
#include <chrono>


using namespace std;
using namespace std::chrono;


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
vector<Point> points;
const int UNDEFINED = 0;
const int MAX_PARTITION_CLUSTER = 1000;
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

double t_comunication = 0;

vector<int> localLabels;

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


void readPoints(char* name)
{
    FILE *f;
    string filename(name);
    string path("../data/");
    string extend(".txt");
    path.append(name);
    path.append(extend);
    f = fopen(path.c_str(), "r");
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
    f.open("../data/merged.txt",ios::trunc);
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

void partitionToFile(int idPartittion)
{
    ofstream f;
    string filename = "../data/partitions/" + to_string(idPartittion) + ".txt";
    f.open(filename,ios::trunc);
    for (int i = 0; i < partitions[idPartittion].size(); i++)
    {
        string px(to_string(partitions[idPartittion][i].x));
        string py(to_string(partitions[idPartittion][i].y));
        string pl(to_string(partitions[idPartittion][i].clusterId));
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
        localLabels.push_back(partitions[idPartition][i].id);
        localLabels.push_back(partitions[idPartition][i].clusterId);
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
        }        // points[partitions[idPartition][i].id].clusters.push_back(partitions[idPartition][i].clusterId);

    }
    for(int i=0;i<n_points;i++){
        points[i].clusterId = root[points[i].clusters[0]];
    }
}

int c; //= 0;

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
        // root.push_back(c);
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
    // partitionToFile(idP);
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

int main(int argc,char** argv)
{
    auto start = high_resolution_clock::now();
    readPoints(argv[1]);
    n_slices = calSlice();
    // cout << "max ox " << max_Ox << endl;
    // cout << "min ox " << min_Oy << endl;
    // cout << "max oy " << max_Oy << endl;
    // cout << "min oy " << min_Oy << endl;
    // cout << "slice ox: " << num_slice_0x << endl;
    // cout << "slice oy: " << num_slice_Oy << endl;
    // cout << "num slices: " << n_slices << endl;

    double start_com,end;


    root.push_back(0);
    partitionPoints();
    // mpi 
    const int ROOT_RANK = 0;
    MPI_Init(NULL,NULL);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);
    int partitionPerProcessor = (n_slices%world_size == 0)?(n_slices/world_size):(n_slices/world_size + 1);
    c = world_rank * 1000;
    for (int i = 0; i < partitionPerProcessor; i++)
    {
        int index = world_rank*partitionPerProcessor + i;
        if(index >= n_slices)
            break;
        dbscan(index);
    };

    int *recvCounts = NULL;
    int *partitionLabels = NULL;

    if(world_rank == ROOT_RANK)
        recvCounts = (int *)malloc( world_size * sizeof(int));
    
    int localSize = localLabels.size();

    start_com = MPI_Wtime();
    MPI_Gather(&localSize,1,MPI_INT,
        recvCounts,1,MPI_INT,
        0,MPI_COMM_WORLD
    );
    end = MPI_Wtime();
    t_comunication += end - start_com;


    int *displs = NULL;
    int totalLen = 0;

    if(world_rank == ROOT_RANK){
        displs = (int *)malloc(world_size * sizeof(int));

        displs[0] = 0;
        totalLen = recvCounts[0];
        int i;
        for(i=1;i<world_size;i++){
            totalLen += recvCounts[i];
            displs[i] = displs[i-1] + recvCounts[i-1];
        }
        partitionLabels = (int *)malloc(totalLen*sizeof(int));
    }

    start_com = MPI_Wtime();
    MPI_Gatherv(
        localLabels.data(),
        localSize,
        MPI_INT,
        partitionLabels,
        recvCounts,
        displs,MPI_INT,ROOT_RANK,
        MPI_COMM_WORLD
    );
    end = MPI_Wtime();
    t_comunication += end - start_com;


    if(world_rank == ROOT_RANK){


        set<int> labels;
        int maxLabel = 0;
        int i,cluster,label_,labelBlock = 0;
        int maxPrevBlock = 0;
        int k = 0;
        for(int i=0;i<totalLen;i+=2){
            label_ = partitionLabels[i+1];

            if(i == displs[k]){
                maxPrevBlock = maxLabel;
                k++;
            }
            cluster = label_ % MAX_PARTITION_CLUSTER + maxPrevBlock;
            maxLabel = max(maxLabel,cluster);

            labels.insert(cluster);
            points[partitionLabels[i]].clusters.push_back(cluster);
        }

        set<int>::iterator it; 
        for(it=labels.begin();it!=labels.end();++it){
            root.push_back(*it);
        }

        updateClusterIdPoints();
        pointsToFile();
        
        free(partitionLabels);
        free(displs);
        free(recvCounts);
    }

    auto stop = high_resolution_clock::now();
    if (world_rank == ROOT_RANK)
    {
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Total time: " << duration.count() << endl;
        cout << "Comunication time: " << t_comunication * 1000000 << endl;
        cout << "Execution time: " << duration.count() - t_comunication * 1000000 << endl;

    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}