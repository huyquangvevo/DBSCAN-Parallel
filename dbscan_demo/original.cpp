#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<iostream>
#include<vector>
#include<cmath>
#include<fstream>
#include <bits/stdc++.h> 
#include<string>

using namespace std;

class Point {
    public:
    float x,y;
    int label;
    bool operator == (const Point &Ref) const{
        return ((this->x == Ref.x) && (this->y == Ref.y) && (this->label == Ref.label));
    }
};

int n_points = 0;
int truth[150000];
float eps = 0.08;
int minPts = 6;
vector<Point> points;
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
        truth[n_points] = strtod(p,NULL);
        p_.label = UNDEFINED;
        points.push_back(p_);
        n_points++;
    };
    fclose(f);
};

void pointsToFile(){
    ofstream f;
    f.open("./data/clustered2.txt");
    for(size_t p=0;p<points.size();++p){
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

vector<Point> rangeQuery(Point p){
    vector<Point> neighbors;
    for (size_t q = 0;q < points.size();++q){
        if(dist2points(p,points[q]) <= eps){
            // if(!comparePoint(points[q],p)){
                // cout << "point 1: " << points[q].x << " " << points[q].y << " " << points[q].label << endl;
                // cout << "point 2: " << p.x << " " << p.y << " " << p.label << endl;
            neighbors.push_back(points[q]);
            // }
        }
    }
    return neighbors;
}

int dbscan(){
    int c = 0;
    for (size_t p = 0; p< points.size();++p){
        if(points[p].label != UNDEFINED)
            continue;
        vector<Point> neighbors =  rangeQuery(points[p]);
        // cout << neighbors.size() << endl;
        // exit(0);
        if(neighbors.size() < minPts){
            points[p].label = -1;
            continue;
        };
        // cout << "neighbors : " << neighbors.size() << endl;
        // exit(0);
        c = c + 1;
        points[p].label = c;
        vector<Point> S = neighbors;
        for(size_t q=0;q<points.size();++q){
            if(points[q].label == -1)
                points[q].label = c;
            if(points[q].label != UNDEFINED)
                continue;
            points[q].label = c;
            vector<Point> N = rangeQuery(points[q]);
            if(N.size()>=minPts){
                // cout << "S size" << S.size() << endl;
                S.insert(S.end(),N.begin(),N.end());
                // cout << "S new size" << S.size() << endl;
            }
        }
    }
    return c;
};

int main(){
    readPoints();
    // int clusters = dbscan();
    cout << "cluster " << clusters << endl;
    pointsToFile();
    int count = 0;
    for (size_t i=0;i<points.size();++i){
        cout << points[i].label << endl;
        // cout << dist2points(points[i],points[i+1]) << endl;
        if(count>10)
            break;
        else 
            count ++;
    }
    return 0;
}