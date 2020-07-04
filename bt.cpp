#include<bits/stdc++.h>
#include<pthread.h>
using namespace std;

// Initialising the locks
pthread_mutex_t s_insert_lock;
pthread_mutex_t s_delete_lock;
pthread_mutex_t d_insert_lock;
pthread_mutex_t d_search_lock;
pthread_mutex_t i_search_lock;
pthread_mutex_t i_delete_lock;

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

// void bt_insert(int ele) {
//     //Lock set by the search function
//     pthread_mutex_lock(&s_insert_lock);
//     //Lock set by the delete function
//     pthread_mutex_lock(&d_insert_lock);
//     cout << "Inserted " << ele << " at level " << (int) log2(bt.size()+1)+1 << endl;
//     bt.push_back(ele);
//     pthread_mutex_unlock(&s_insert_lock);
//     pthread_mutex_unlock(&d_insert_lock);
// }

void* bt_insert(void * args) {
    //Lock set by the search function
    pthread_mutex_lock(&s_insert_lock);
    //Lock set by the delete function
    pthread_mutex_lock(&d_insert_lock);
    int *ele = (int*)args;
    cout << "Inserted " << *ele << " at level " << (int) log2(bt.size()+1)+1 << endl;
    bt.push_back(*ele);
    pthread_mutex_unlock(&d_insert_lock);
    pthread_mutex_unlock(&s_insert_lock);
    return NULL;
}

void* bt_delete(void* args) {
    //Lock set by the search function
    pthread_mutex_lock(&s_delete_lock);
    //Lock set by the insert function
    pthread_mutex_lock(&i_delete_lock);
    int *ele = (int*)args;
    if(bt.size() == 0) {
        cout << "Binary tree is empty!" << endl;
        return NULL;
    }
    for(vector<int>::iterator i = bt.begin(); i != bt.end(); i++) {
        if(*i == *ele) {
            if(i != bt.end()-1) {
                *i = bt.back();
                cout << "Deleted " << *ele << " at level " << (int) log2(i-bt.begin()+1)+1 << " and replaced it with " << bt.back() << endl;
            } else {
                cout << "Deleted " << *ele << " at level " << (int) log2(i-bt.begin()+1)+1 << endl;
            }
            bt.pop_back();
            return NULL;
        }
    }
    cout << *ele << " not found in the binary tree!" << endl;
    pthread_mutex_unlock(&i_delete_lock);
    pthread_mutex_unlock(&s_delete_lock);
    return NULL;
}

// int bt_search(int ele) {
//     for(vector<int>::iterator i = bt.begin(); i != bt.end(); i++) {
//         if(*i == ele) {
//             return 1;
//         }
//     }
//     return 0;
// }

void* bt_search(void *args)
{
    //Lock set by the delete function
    pthread_mutex_lock(&d_search_lock);
    //Lock set by the insert function
    pthread_mutex_lock(&i_search_lock);
    int *ele = (int*)args;
    int *result = (int *)malloc(sizeof(int));
    *result = 0;

    for(vector<int>::iterator i = bt.begin(); i != bt.end(); i++) {
        if(*i == *ele) {
            *result = 1;
        }
    }
    pthread_mutex_unlock(&i_search_lock);
    pthread_mutex_unlock(&d_search_lock);
    return result;
}


int main() {
    // bt_insert(10);
    // bt_insert(9);
    // bt_insert(7);
    // bt_insert(5);
    // bt_insert(14);
    // bt_insert(21);
    // bt_level_order_traversal(); 
    // bt_delete(9);
    // bt_level_order_traversal();
    // bt_delete(100);
    // if(bt_search(10)) {
    //     cout << 10 << " found" << endl;
    // } else {
    //     cout << 10 << " not found" << endl;
    // }
    // bt_delete(10);
    // if(bt_search(10)) {
    //     cout << 10 << " found" << endl;
    // } else {
    //     cout << 10 << " not found" << endl;
    // }
    // bt_level_order_traversal();
    // bt_delete(9);
    // bt_level_order_traversal();
    // bt_delete(7);
    // bt_level_order_traversal();
    // bt_delete(5);
    // bt_level_order_traversal();
    // bt_delete(14);
    // bt_level_order_traversal();
    // bt_delete(21);
    // bt_level_order_traversal();
    // bt_delete(21);
    pthread_t s1,d1, i1;
    int choice, ele;
    int *found;
    while(1)
    {
        cin>>choice;
        switch(choice)
        {
            case 1 : cin>>ele;
                     pthread_mutex_init(&i_delete_lock, NULL);
                     pthread_mutex_init(&i_search_lock, NULL);
                     pthread_create(&i1, NULL, bt_insert, &ele);
                     pthread_join(i1, NULL);
                     pthread_mutex_destroy(&i_delete_lock);
                     pthread_mutex_destroy(&i_search_lock);
                     break;

            case 2 : cin>>ele;
                     pthread_mutex_init(&d_insert_lock, NULL);
                     pthread_mutex_init(&d_search_lock, NULL);
                     pthread_create(&d1, NULL, bt_delete, &ele);
                     pthread_join(d1, NULL);
                     pthread_mutex_destroy(&d_insert_lock);
                     pthread_mutex_destroy(&d_search_lock);
                     break;

            case 3 : cin>>ele;
                     pthread_mutex_init(&s_insert_lock, NULL);
                     pthread_mutex_init(&s_delete_lock, NULL);
                     pthread_create(&s1, NULL, bt_search, &ele);
                     pthread_join(s1, (void **)&found);
                     pthread_mutex_destroy(&s_insert_lock);
                     pthread_mutex_destroy(&s_delete_lock);
                     cout<<*found;
                     //cout<<bt_search(ele)<<"\n";
                     break;

            case 4 : bt_level_order_traversal();
                     break;

            default : exit(0);
        }
    }
    return 0;
}