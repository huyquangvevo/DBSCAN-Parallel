#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<iostream>
#include<vector>
#include<cmath>
#include<fstream>
#include <bits/stdc++.h> 
#include<string>
#include<set>
#include<algorithm>

using namespace std;

class Point {
    public:
    float x,y;
    int clusterId;
    bool isLocalRegion;
    bool isCorePoint;
    int id;
};

int n_points = 0;
float eps = 0.3;
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


void readPoints(){
    FILE *f;
    f = fopen("./data/Circle.txt","r");
    char point[255];
    while (fgets(point,255,f)){
        char* p = strtok(point," ");
        Point p_;
        p_.x = strtof(p,NULL);
        p = strtok(NULL," ");
        p_.y = strtof(p,NULL);
        p = strtok(NULL," ");
        max_Ox = max(max_Ox,p_.x);
        min_Ox = min(min_Ox,p_.x);
        max_Oy = max(max_Oy,p_.y);
        min_Oy = min(min_Oy,p_.y);
        p_.clusterId = UNDEFINED;
        p_.isLocalRegion = false;
        p_.isCorePoint = false;
        p_.id = n_points;
        // points[n_points] = p_;
        points.push_back(p_);
        n_points++;
    };
    fclose(f);
};

int calSlice(){
    num_slice_0x = int(round((max_Ox - min_Ox) / eps));
    num_slice_Oy = int(round((max_Oy - min_Oy) / eps));
    return num_slice_0x * num_slice_Oy;
}

void pointsToFile(){
    ofstream f;
    f.open("./data/clustered2.txt");
    for(int p=0;p<n_points;p++){
        string px(to_string(points[p].x));
        string py(to_string(points[p].y));
        string pl(to_string(points[p].clusterId));
        string pstr = px + " " + py + " " + pl +"\n";
        f << pstr;
    }
    f.close();
}

float dist2points(Point p1,Point p2){
    return sqrt(pow((p2.x - p1.x),2) + pow((p2.y-p1.y),2));
}

bool comparePoint(Point p1,Point p2){
    return ((p1.x == p2.x) && (p1.y == p2.y) && (p1.clusterId == p2.clusterId));
}

set<int> rangeQuery(Point p){
    set<int> neighbors;
    for (int q=0;q < n_points;q++){
        if(dist2points(p,points[q]) <= eps){
                neighbors.insert(points[q].id);
        }
    }
    return neighbors;
}

// int dbscan(set<int> points){
//     int c = 0;
//     for (size_t p = 0; p < points.size();++p){
//         if(points[p].clusterId != UNDEFINED)
//             continue;
//         set<int> neighbors = rangeQuery(points[p]);
//         if(neighbors.size() < minPts){
//             points[p].clusterId = -1;
//             continue;
//         };
//         c = c + 1;
//         points[p].clusterId = c;
//         set<int> S = neighbors;
//         while (S.size()>0)
//         {
//             set<int>::iterator i = S.begin();
//             S.erase(i);
//             if(points[*i].clusterId == -1){
//                 points[*i].clusterId = c;
//             }
//             if(points[*i].clusterId != UNDEFINED)
//                 continue;
//             points[*i].clusterId = c;
//             set<int> N = rangeQuery(points[*i]);
//             if(N.size()>=minPts){
//                 S.insert(N.begin(),N.end());
//             }
//         }
//     }
//     return c;
// };


void partitionPoints(){
    int sX,sY,slice;
    // float shift_x,shift_y;
    for(int i=0;i<n_points;i++){
        sX = int(floor((points[i].x - min_Ox) / eps));
        sY = int(floor((points[i].y - min_Oy) / eps));
        // cout << "ox : " << points[i].x << endl;
        // cout << "oy : " << points[i].y << endl;
        // cout << sX << " - " << sY << endl;
        slice = num_slice_0x*sY + sX;
        // cout << "slice index: " << slice << endl;
        // exit(0);
        partitions[slice].push_back(points[i]);
    }
}

int main(){
    readPoints();
    n_slices = calSlice();
    cout << "max ox " << max_Ox << endl;
    cout << "min ox " << min_Oy << endl;
    cout << "max oy " << max_Oy << endl;
    cout << "min oy " << min_Oy << endl;
    cout << num_slice_0x << endl;
    cout << num_slice_Oy << endl;
    cout << n_slices << endl;
    partitionPoints();
    for(int i=0;i<n_slices;i++){
        cout << "Partition " << i << " : " << partitions[i].size() << endl;
    }
    return 0;
    // int clusters = dbscan();
    // cout << "cluster " << clusters << endl;
    // pointsToFile();
    // int count = 0;
    // for (int i=0;i<n_points;i++){
    //     cout << points[i].clusterId << endl;
    //     if(count>10)
    //         break;
    //     else 
    //         count ++;
    // }
    // return 0;
}