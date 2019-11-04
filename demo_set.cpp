#include<set>
#include<iostream>
// #include<std>

using namespace std ;

int main(){
    set<int> points;
    points.insert(0);
    points.insert(3);
    points.insert(2);
    points.insert(5);
    points.insert(2);
    points.insert(3);
    set<int> points2;
    points2.insert(20);
    points2.insert(30);
    points2.insert(50);
    points2.insert(40);
    bool con = false;
    for (set<int>::iterator i=points.begin();i!=points.end();++i){
        cout << *i << endl;
        if (!con){
            cout << "append" << endl;
            points.insert(points2.begin(),points2.end());
            con = !con;
        } else {
            con = !con;
        }
    }


}