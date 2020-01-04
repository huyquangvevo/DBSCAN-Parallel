#include "matplotlibcpp.h"
#include<iostream>
namespace plt = matplotlibcpp;
using namespace std;
int main() {
    cout << "matplotlib cpp " << endl;
    plt::plot({1,3,2,4});
    plt::show();
}