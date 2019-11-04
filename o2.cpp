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
};

int n_points = 0;
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
        p_.label = UNDEFINED;
        p_.id = n_points;
        points[n_points] = p_;
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
    for (int q=0;q < n_points;q++){
        if(dist2points(p,points[q]) <= eps){
                neighbors.insert(points[q].id);
        }
    }
    return neighbors;
}

int dbscan(){
    int c = 0;
    for (int p = 0; p < n_points;p++){
        if(points[p].label != UNDEFINED)
            continue;
        set<int> neighbors = rangeQuery(points[p]);
        if(neighbors.size() < minPts){
            cout << "noise" << endl;
            points[p].label = -1;
            continue;
        };
        c = c + 1;
        points[p].label = c;
        set<int> S = neighbors;
        while (S.size()>0)
        {
            set<int>::iterator i = S.begin();
            S.erase(i);
            if(points[*i].label == -1){
                points[*i].label = c;
                cout << "noise" << endl;
            }
            if(points[*i].label != UNDEFINED)
                continue;
            points[*i].label = c;
            set<int> N = rangeQuery(points[*i]);
            if(N.size()>=minPts){
                S.insert(N.begin(),N.end());
            }
        }
    }
    return c;
};

int main(){
    readPoints();
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