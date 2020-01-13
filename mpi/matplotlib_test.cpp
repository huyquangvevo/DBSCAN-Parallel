#include <cmath>
#include "matplotlibcpp.h"
#include <iostream>
#include <set>
#include <unordered_map> 

using namespace std;
namespace plt = matplotlibcpp;

int main() 
{    
    // Prepare data.
    int n = 5000; // number of data points
    vector<double> x,y;
    set<int> l;
    FILE *f;
    f = fopen("../data/merged.txt", "r");
    char point[255];
    while (fgets(point, 255, f))
    {
        char *p = strtok(point, " ");
        // Point p_;
        // cout << strtof(p, NULL) << endl;
        x.push_back(strtof(p, NULL));
        p = strtok(NULL, " ");
        y.push_back(strtof(p, NULL));
        p = strtok(NULL, " ");
        // l.push_back(strtof(p, NULL));
        l.insert(strtof(p, NULL));
        p = strtok(NULL, " ");
    };
    fclose(f);

    // for(int i=0; i<x.size(); ++i) {
    //     cout << x[i] << endl;
    // }

    // plot() takes an arbitrary number of (x,y,format)-triples. 
    // x must be iterable (that is, anything providing begin(x) and end(x)),
    // y must either be callable (providing operator() const) or iterable. 
    // plt::plot(x, y, "r-", x, [](double d) { return 12.5+abs(sin(d)); }, "k-");
    unordered_map<string, string> umap;
    umap["l"] = "1"; 
 
    plt::scatter(x,y,10);


    // show plots
    plt::show();
} 