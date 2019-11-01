#include<stdio.h>
#include<string.h>
#include<stdlib.h>

struct Point
{
    float x,y;
    int label;
};

int n_points = 0;
int truth[150000];
struct Point points[150000];

void readPoints(){
    FILE *f;
    f = fopen("./data/Circle.txt","r");
    char point[255];
    while (fgets(point,255,f)){
        char* p = strtok(point," ");
        struct Point p_;
        p_.x = strtof(p,NULL);
        p = strtok(NULL," ");
        p_.y = strtof(p,NULL);
        p = strtok(NULL," ");
        truth[n_points] = strtod(p,NULL);
        p_.label = NULL
        points[n_points] = p_;
        n_points++;
    };
    printf("%d", points[0].label);
    fclose(f);
}

dbscan(){

}

main(){
    readPoints();
}