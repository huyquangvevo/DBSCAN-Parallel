#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<iostream>
#include<vector>
#include<cmath>

using namespace std;

class Point {
    public:
    float x,y;
    int label;
};

int n_points = 0;
int truth[150000];
float eps = 4;
int minPts = 10;
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
}

float dist2points(Point p1,Point p2){
    return sqrt(pow((p2.x - p1.x),2) + pow((p2.y-p1.y),2));
}

vector<Point> rangeQuery(Point p){
    vector<Point> neighbors;
    for (size_t q = 0;q < points.size();++q){
        if(dist2points(p,points[q]) <= eps){
            neighbors.push_back(points[q]);
        }
    }
    return neighbors;
}

void dbscan(){
    int c = 0;
    for (size_t p = 0; p< points.size();++p){
        if(points[p].label != UNDEFINED)
            continue;
        vector<Point> neighbors =  rangeQuery(points[p]);
        if(neighbors.size() + 1 < minPts){
            points[p].label = -1;
            continue;
        };
        c = c + 1;
        points[p].label = c;
        vector<Point> S = neighbors;
        for(size_t q=0;q<points.size();++q){
            if(points[q].label == -1)
                points[q].label = c;
            if(points[q].label != UNDEFINED)
                continue;
            vector<Point> N = rangeQuery(points[q]);
            if(N.size()+1>=minPts){
                S.insert(S.end(),N.begin(),N.end());
            }
        }
    }
};

int main(){
    readPoints();
    // dbscan();
    int count = 0;
    for (size_t i=0;i<points.size();++i){
        cout << points[i].label << endl;
        if(count>10)
            break;
        else 
            count ++;
    }
    return 0;
}