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

using namespace std;

class Point {
    public:
    float x,y;
    int label;
    int id;
    bool visited;
};

int n_points = 0;
// int truth[150000];
float eps = 0.3;
int minPts = 5;
Point points[150000];
const int UNDEFINED = -100;


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
        // truth[n_points] = strtod(p,NULL);
        p_.label = UNDEFINED;
        p_.visited = false;
        p_.id = n_points;
        points[n_points] = p_;
        // points.push_back(p_);
        n_points++;
    };
    fclose(f);
};

void pointsToFile(){
    ofstream f;
    f.open("./data/clustered2.txt");
    for(int p=0;p<n_points;p++){
        string px(to_string(points[p].x));
        string py(to_string(points[p].y));
        string pl(to_string(points[p].label));
        string pstr = px + " " + py + " " + pl +"\n";
        f << pstr;
    }
    f.close();
}

float dist2points(Point p1,Point p2){
    return sqrt(pow((p2.x - p1.x),2) + pow((p2.y-p1.y),2));
}

bool comparePoint(Point p1,Point p2){
    return ((p1.x == p2.x) && (p1.y == p2.y) && (p1.label == p2.label));
}

set<int> rangeQuery(Point p){
    set<int> neighbors;
    for (int q_index=0; q_index < n_points; q_index++){
        if (dist2points(p, points[q_index]) <= eps){
            neighbors.insert(points[q_index].id);                
        }
    }
    return neighbors;
}



int dbscan(){
    int cluster_id = 0;
    for (int p_index=0; p_index < n_points; p_index++){
        if(points[p_index].visited){
            continue;
        }
        points[p_index].visited = true;

        set<int> neighbors = rangeQuery(points[p_index]);

        if(neighbors.size() < minPts){
            points[p_index].label = -1;
        }
        else{
            points[p_index].label = cluster_id;
            while(neighbors.size() > 0){
                set<int>::iterator x_index_p = neighbors.begin();
                neighbors.erase(x_index_p);
                if(!points[*x_index_p].visited){
                    points[*x_index_p].visited = true;
                    set<int> neighbors_x = rangeQuery(points[*x_index_p]);
                    if(neighbors_x.size() >= minPts){
                        neighbors.insert(neighbors_x.begin(), neighbors_x.end());
                    }
                }
                if(points[*x_index_p].label < 0){
                    points[*x_index_p].label = cluster_id;
                    points[*x_index_p].visited = true;
                }
            }
            cluster_id++;
        }
        cout << "vinhsuhi" << endl;
    }
    return 0;
}

int main(){
    readPoints();
    // return 0;
    int clusters = dbscan();
    cout << "cluster " << clusters << endl;
    pointsToFile();
    int count = 0;
    for (int i=0;i<n_points;i++){
        cout << points[i].label << endl;
        if(count>10)
            break;
        else 
            count ++;
    }
    return 0;
}