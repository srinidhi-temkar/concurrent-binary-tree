#include<bits/stdc++.h>
using namespace std;

vector<int> bt;

//        10
//      /    \
//     /      \
//    9        7
//  /   \     / 
// 5    14  21

void bt_level_order_traversal() {
    if(bt.size() == 0) {
        cout << "Binary tree is empty!" << endl;
        return;
    }
    cout << "Level order traversal of the binary tree : ";
    for(int x : bt) {
        cout << x << " ";
    }
    cout << endl;
}

void bt_insert(int ele) {
    cout << "Inserted " << ele << " at level " << (int) log2(bt.size()+1)+1 << endl;
    bt.push_back(ele);
}

void bt_delete(int ele) {
    if(bt.size() == 0) {
        cout << "Binary tree is empty!" << endl;
        return;
    }
    for(vector<int>::iterator i = bt.begin(); i != bt.end(); i++) {
        if(*i == ele) {
            if(i != bt.end()-1) {
                *i = bt.back();
                cout << "Deleted " << ele << " at level " << (int) log2(i-bt.begin()+1)+1 << " and replaced it with " << bt.back() << endl;
            } else {
                cout << "Deleted " << ele << " at level " << (int) log2(i-bt.begin()+1)+1 << endl;
            }
            bt.pop_back();
            return;
        }
    }
    cout << ele << " not found in the binary tree!" << endl;
}

int main() {
    bt_insert(10);
    bt_insert(9);
    bt_insert(7);
    bt_insert(5);
    bt_insert(14);
    bt_insert(21);
    bt_level_order_traversal(); 
    bt_delete(9);
    bt_level_order_traversal();
    bt_delete(100);
    bt_delete(10);
    bt_level_order_traversal();
    bt_delete(9);
    bt_level_order_traversal();
    bt_delete(7);
    bt_level_order_traversal();
    bt_delete(5);
    bt_level_order_traversal();
    bt_delete(14);
    bt_level_order_traversal();
    bt_delete(21);
    bt_level_order_traversal();
    bt_delete(21);
}