#include <iostream>
#include <vector>
#include <cmath>
#include <pthread.h>
using namespace std;

// Macro to select mode of operation.
// If left commented (recommended and default), the threads are created as soon as each 
// input is read. This provides a greater control on the sequence of operations. It is usually faster. 
// If uncommented, all the inputs are collected first, stored in a vector and then threads
// are created based on values in the vector. This is useful for detecting concurrency bugs
// more easily, but provides lesser control on the sequence of operations. It is usually slower. 
// Note: In both the cases, the selection of the specific thread to execute is non-deterministic 
// and left to the kernel. Requires re-compilation if changed.

// #define COLLECT_ALL_INPUTS_FIRST

vector<long> bt; // Global vector variable for the binary tree (Array Representation)

// ReadWrite lock for the binary tree
pthread_rwlock_t bt_rwlock; 

// Mutex to protect the output buffer while printing the tree 
// Prevent atomicity violations in bt_level_order_traversal function
pthread_mutex_t cout_mutex; 

/*
 bt_insert has an amortized O(1) time complexity, owing to the array representation.
*/
void* bt_insert(void *arg) {
	// casting the void* argument received during thread creationg to long
	long value = (long) arg;

	int level, position; // 1-based 

	// Acquiring the write lock 
	pthread_rwlock_wrlock(&bt_rwlock);

	// Calculating the level and position of the insertion
	level = (int) log2(bt.size()+1)+1;
	position = bt.size()+2 - (int) (pow(2, level-1)+0.5);

	// Aquired the write lock and hence locking of cout mutex is not required
	cout << "Inserted " << value << " at level:" << level << ",position:" << position << endl;
	
	// inserting the new element at the end of the vector
	bt.push_back(value);

	// Releasing the write lock and exiting the thread
	pthread_rwlock_unlock(&bt_rwlock);
	return NULL;
}

/* 
 bt_delete deletes the first occurence of a specified value in the binary tree, by replacing
 its content with the last node's content (if it isn't already the last node).
 For this to always be the first occurence and to make sure that if the value exists, it is 
 definitely deleted, the function is made to work on a snapshot of the tree by traversing
 the tree to find the node only after acquiring the write lock.
 If the traversal was made using hand-over-hand locking technique, i.e., if the search for the
 matching node was made without locking the mutex, and the mutex was locked only after finding
 the node, then it would lead to a logic error if 2 threads were trying to delete 2 different nodes, 
 with one of them being the last node. See README for details.
*/
void* bt_delete(void *arg) {
	// casting the void* argument received during thread creationg to long
	long value = (long) arg;

	// Acquiring the read lock
	pthread_rwlock_rdlock(&bt_rwlock);
	if(bt.size() == 0) { // checking if the tree is empty

		// Locking the cout mutex to prevent writing into the buffer when another read-thread is printing the tree
		pthread_mutex_lock(&cout_mutex);

		cout << "Delete " << value << ": The binary tree is empty!" << endl;

		// Unlocking the cout mutex
		pthread_mutex_unlock(&cout_mutex);

		// Releasing the read lock and exiting the thread
		pthread_rwlock_unlock(&bt_rwlock);
		return NULL;
	}
	// Releasing the read lock to acquire the write lock
	pthread_rwlock_unlock(&bt_rwlock);

	int level, position; // 1-based

	// Acquiring the write lock
	pthread_rwlock_wrlock(&bt_rwlock);

	// traversing the binary tree to locate the matching node, if any
	for(vector<long>::iterator i = bt.begin(); i < bt.end(); i++) { 
		if(*i == value) {

			// if found, calculating its level and position
			level = (int) log2(i-bt.begin()+1)+1;
			position = i-bt.begin()+2 - (int) (pow(2, level-1)+0.5);

			// Aquired the write lock and hence locking of cout mutex is not required
			cout << "Deleted " << value << " at level:" << level << ",position:" << position;
			
			// if the node chosen to be deleted is not already at the end, replacing its content 
			// with the content of the last node
			if(i != bt.end()-1) { 
				*i = bt.back();
				cout << " and replaced it with " << bt.back() << endl;
			} else { // node to be deleted is already the last node
				cout << endl;
			}

			// Deleting the last node
			bt.pop_back();

			// Releasing the write lock and exiting the thread after deletion
			pthread_rwlock_unlock(&bt_rwlock);
			return NULL;
		}
	} // node to be deleted doesn't exist

	// Aquired the write lock and hence locking of cout mutex is not required
	cout << "Delete " << value << ": Not found in the binary tree!" << endl;

	// Releasing the write lock and exiting the thread with no modifications done to the tree
	pthread_rwlock_unlock(&bt_rwlock);
	return NULL;
}

/* 
 bt_search function is not made to return an int value, but rather print the appropriate
 message in the function itself. This is done to make sure that the main thread focuses 
 only on obtaining the inputs, which unnecessitates the need of joining each new search 
 thread right after creation. This increases the extent of concurrency as multiple searches
 can happen at the same time, which may in turn reduce wait time and increase performance.
*/
void* bt_search(void *arg) {
	// casting the void* argument received during thread creationg to long
	long value = (long) arg;
	
	int level, position; // 1-based

	// Acquiring the read lock
	pthread_rwlock_rdlock(&bt_rwlock);
	if(bt.size() == 0) { // checking if the tree is empty

		// Locking the cout mutex to prevent writing into the buffer when another read-thread is printing the tree
		pthread_mutex_lock(&cout_mutex);

		cout << "Search " << value << ": The binary tree is empty!" << endl;

		// Unlocking the cout mutex
		pthread_mutex_unlock(&cout_mutex);

		// Releasing the read lock and exiting the thread
		pthread_rwlock_unlock(&bt_rwlock);
		return NULL;
	}

	// traversing the binary tree to locate the matching node, if any
	for(vector<long>::iterator i = bt.begin(); i < bt.end(); i++) {
		if(*i == value) { 

			// if found, calculating its level and position
			level = (int) log2(i-bt.begin()+1)+1;
			position = i-bt.begin()+2 - (int) (pow(2, level-1)+0.5);

			// Locking the cout mutex to prevent writing into the buffer when another read-thread is printing the tree
			pthread_mutex_lock(&cout_mutex);

			cout << "Search " << value << ": Found at level:" << level << ",position:" << position << endl;

			// Unlocking the cout mutex
			pthread_mutex_unlock(&cout_mutex);

			// Releasing the read lock and exiting the thread
			pthread_rwlock_unlock(&bt_rwlock);
			return NULL;
		}
	}

	// Locking the cout mutex to prevent writing into the buffer when another read-thread is printing the tree
	pthread_mutex_lock(&cout_mutex);

	cout << "Search " << value << ": Not found in the binary tree!" << endl;

	// Unlocking the cout mutex
	pthread_mutex_unlock(&cout_mutex);

	// Releasing the read lock and exiting the thread
	pthread_rwlock_unlock(&bt_rwlock);
	return NULL;
}

/*
 Prints the binary tree in a level order fashion. 
 While traversing the tree, if the tree is not empty, then after each element is 
 printed, a space is inserted and the cout statement is ended. If cout_mutex 
 was not used, here and in other functions, then before the next element would be
 printed, another thread could have ended up printing to the output buffer.
 (Atomicity violation pitfall)
*/
void* bt_level_order_traversal(void *null) {

	// Acquiring the read lock
	pthread_rwlock_rdlock(&bt_rwlock);

	if(bt.size() == 0) { // checking if the tree is empty

		// Locking the cout mutex to prevent writing into the buffer when another read-thread is printing the tree
		pthread_mutex_lock(&cout_mutex);

		cout << "Print binary tree: The tree is empty!" << endl;

		// Unlocking the cout mutex
		pthread_mutex_unlock(&cout_mutex);

		// Releasing the read lock and exiting the thread
		pthread_rwlock_unlock(&bt_rwlock);
		return NULL;
	}

	// Locking the cout mutex to prevent any other thread from writing into the buffer 
	pthread_mutex_lock(&cout_mutex);

	cout << "Level order traversal of the binary tree : ";
	for(long x : bt) { // Traversing and printing each element of the tree
		cout << x << " ";
	}
	cout << endl;

	// Unlocking the cout mutex
	pthread_mutex_unlock(&cout_mutex);

	// Releasing the read lock and exiting the thread
	pthread_rwlock_unlock(&bt_rwlock);
	return NULL;
}

int main() {

	// initializing the ReadWrite lock attribute
	pthread_rwlockattr_t rw_attr;
	pthread_rwlockattr_init(&rw_attr);

	#ifdef COLLECT_ALL_INPUTS_FIRST
		// Setting rwlockattr to PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP to prevent writer starvation
		// Avaliable in most implementations
		// Can also be set to PTHREAD_RWLOCK_PREFER_READER_NP based on application to prevent reader starvation.
		// To obtain effects similar to in-order priority, simply UNcomment the macro COLLECT_ALL_INPUTS_FIRST,
		// which will create the thread as soon as it reads the input
		pthread_rwlockattr_setkind_np(&rw_attr, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);
	#endif
	
	// initializing the locks
	pthread_rwlock_init(&bt_rwlock, &rw_attr);
	pthread_mutex_init(&cout_mutex, NULL);
	
	int num_operations, choice;
	long value;

	// initializing the pthread attribute attr
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	// explicitly setting the detachstate for thread creation to joinable as not all implementaions create joinable threads by default
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	cin >> num_operations; // make sure this does not exceed your implementaion's thread limit (usually of the order of thousands)
	pthread_t threads[num_operations]; // array of threads

	#ifdef COLLECT_ALL_INPUTS_FIRST
		vector<pair<int, long>> inputs;

		// storing all the inputs before thread creation to make it easier to find concurrency bugs, if any
		for(int i=0; i<num_operations; i++) {
			cin >> choice;
			switch(choice) {
				case 1: // insert
						cin >> value;
						inputs.push_back(make_pair(choice, value));
						break;

				case 2: // delete
						cin >> value;
						inputs.push_back(make_pair(choice, value));
						break;

				case 3: // search
						cin >> value;
						inputs.push_back(make_pair(choice, value));
						break;
				
				case 4: // print in level order fashion
						inputs.push_back(make_pair(choice, 0));
						break;
				
				default: // invalid
						cout << "Invalid operation code entered" << endl;
						cout << "Exiting main with code 1" << endl;
						exit(1);
			}
		}
	#endif

	for(int t=0; t<num_operations; t++) {
		#ifdef COLLECT_ALL_INPUTS_FIRST
			switch(inputs[t].first) {
		#endif
		#ifndef COLLECT_ALL_INPUTS_FIRST
			cin >> choice;
			switch(choice) {
		#endif
			case 1: // insert
					#ifdef COLLECT_ALL_INPUTS_FIRST
						pthread_create(&threads[t], &attr, bt_insert, (void*) inputs[t].second);
					#endif
					#ifndef COLLECT_ALL_INPUTS_FIRST
						cin >> value;
						pthread_create(&threads[t], &attr, bt_insert, (void*) value);
					#endif
					break;

			case 2: // delete
					#ifdef COLLECT_ALL_INPUTS_FIRST
						pthread_create(&threads[t], &attr, bt_delete, (void*) inputs[t].second);
					#endif
					#ifndef COLLECT_ALL_INPUTS_FIRST
						cin >> value;
						pthread_create(&threads[t], &attr, bt_delete, (void*) value);
					#endif
					break;

			case 3: // search
					#ifdef COLLECT_ALL_INPUTS_FIRST
						pthread_create(&threads[t], &attr, bt_search, (void*) inputs[t].second);
					#endif
					#ifndef COLLECT_ALL_INPUTS_FIRST
						cin >> value;
						pthread_create(&threads[t], &attr, bt_search, (void*) value);
					#endif
					break;

			case 4: // print in level order fashion
					pthread_create(&threads[t], &attr, bt_level_order_traversal, NULL);
					break;

			default: // invalid
					cout << "Invalid operation code entered" << endl;
					cout << "Exiting main with code 1" << endl;
					exit(1);
		}
	}

	// Joining all threads to the main thread to make sure all of them have completed execution
	for(int t=0; t < num_operations; t++) {
		pthread_join(threads[t], NULL);
	}

	// Cleaning up
	pthread_rwlockattr_destroy(&rw_attr);
	pthread_attr_destroy(&attr);
	pthread_rwlock_destroy(&bt_rwlock);
	pthread_mutex_destroy(&cout_mutex);
	pthread_exit(NULL);
	return 0;
}