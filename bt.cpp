#include<bits/stdc++.h>
using namespace std;

vector<int> bt;

//        10
//      /    \
//     /      \
//    9        7
//  /   \     / 
// 5    14  21

void printLevelOrderTraversal(vector<int> &bt) {
    cout << "Level order traversal of the binary tree : " << endl;
    for(int x : bt) {
        cout << x << " ";
    }
    cout << endl;
}

void bt_insert(int ele) {
    bt.push_back(ele);
}

void bt_delete(int ele) {
    for(vector<int>::iterator i = bt.begin(); i != bt.end(); i++) {
        if(*i == ele) {
            *i = bt.back();
            bt.pop_back();
        }
    }
}

int main() {
    bt_insert(10);
    bt_insert(9);
    bt_insert(7);
    bt_insert(5);
    bt_insert(14);
    bt_insert(21);
    printLevelOrderTraversal(bt);    
    bt_delete(9);
    printLevelOrderTraversal(bt);    
}