# Concurrent Binary Tree
### Implementation of a concurrent version of a binary tree data structure using the pthread library in c++
This implementation of a binary tree uses an array representation (sequential representation). To allow dynamic changes in the size of the array, a c++ ```std::vector<long>``` is used to store the binary tree. The tree is protected from concurrency bugs using a pthread_rwlock (ReadWrite lock). 

This type of lock allows a number of readers or at most one writer at any point in time. The write portion of this lock allows modification of the underlying data (exclusive access) and the read portion of this lock allows for read-only access (shared access).

In comparison, a Mutex does not distinguish between readers or writers that acquire the lock, therefore blocking any threads waiting for the lock to become available. An RwLock will allow any number of readers to acquire the lock as long as a writer is not holding the lock. This enables multiple threads to parallely read the tree.

The advantages of array-vector representation include lower memory requirements as no pointers are stored, faster searches and faster printing of the tree due to faster traversals.

### Level and Position
The bt_insert, bt_delete and bt_search functions below print the level and position in that level of specific node in the binary tree.
Both the level and position follow 1-based indexing.
Eg: 
```
     1
   /   \
  2     3
 / \   / \
4   5 6   7
```
Here, 

1 is at level:1, position:1

2 is at level:2, position:1

5 is at level:3, position:2

## Functions implemented
### bt_insert
As the binary tree is implemented using a vector, insertion into the tree has an amortized O(1) time complexity. If a dynamic node representation was used, then achieving a constant time complexity would be slightly harder as it would require a pointer to the last node at all times, which means insertion and deletion operations should keep updating the last node pointer.
It prints the level and the position in that level at which the given node is inserted. It acquires a write lock to ensure no other writes happen concurrently. 

### bt_delete
This function deletes the first occurence of a specified value in the binary tree, by replacing its content with the last node's content (if it isn't already the last node), and deletes the last node. The deletion is made on an instance/snapshot of the tree i.e., no other writes are allowed when traversing the tree to find the node to be deleted. This is done to ensure that there are no concurrency bugs as well as logic errors.

If the traversal was made using hand-over-hand locking technique, i.e., if the search for the node to be deleted was made after only acquiring the read lock, then it would lead to a logic error.
Consider the following binary tree:
```
     1
   /   \
  2     3
 / \   / \
4   5 6   7
```
Now, suppose 2 threads are trying to execute the delete function on node 3 and node 7 respectively. Now, if only the read lock was acquired while traversing, then suppose the second thread has reached node 5, and hasn't found 7 yet. Now, if the context switches to the first node and it traverses the tree and finds the node 3, replaces its content with the last node's content (7), and finally deletes the last node, then the thread has completed its execution and exits. When the second thread resumes, it traverses to the next node, 6 which is also the new last node. As there are no more nodes, it exits as well.
The final binary tree would look like:
```
     1
   /   \
  2     7
 / \   / 
4   5 6   
```
This presents a logic error as the second thread was unable to delete the node 7 even though it was present at all times in the tree when the thread was being executed.

The function first acquires the read lock before acquiring the write lock to check if the binary tree empty. 

### bt_search
The function checks if the specified node is present in the binary tree. As it only acquires a read lock, multiple searches can happen simultaneously. Also, multiple searches can happen even when the tree is being printed by the next function. But the search thread will have to wait if has to print to the output buffer while another thread is printing the tree. This is ensured by the cout mutex.

### bt_level_order_traversal
This function prints the tree in a level order fashion. While writing into the output buffer, it locks the cout mutex to make sure no other read-thread writes into the output buffer while printing. This is done to make sure that there is no atomic violation.

## About test cases
* **t1.txt** file consisting of test cases which handle basic operations of the binary tree, this text case file gives importance on reflecting the appropriate working of all the basic operations in binary tree.

* **t2.txt** file consisting of test cases which handle edge cases and reflecting the importance on how the mutex locks handle to prevent writing into the buffer when another read-thread is printing the tree.

* **t3.txt** file consisting of test cases to reflect the usage of read-write locks.

* **t4.txt** test case handles large number of insertion and deletion operations.

* **t5.txt** file includes large number of operations performed in random.

## Compiling the code
* **Compiling**

     First select the mode of operation to collect the input from the input buffer. This is to be done by commenting or uncommenting the Macro COLLECT_ALL_INPUTS_FIRST at the top of bt.cpp file.
     If left commented (recommended and default), the threads are created as soon as each input is read. This provides a greater control on the sequence of operations. It is usually faster. 
     If uncommented, all the inputs are collected first, stored in a vector and then threads are created based on values in the vector. This is useful for detecting concurrency bugs more easily, but provides lesser control on the sequence of operations. It is usually slower. 
     **Note** In both the cases, the selection of the specific thread to execute is non-deterministic and left to the kernel. Requires re-compilation if changed.

     Then, execute: 

      g++ -o bt bt.cpp -lpthread
      
* **Running the executable** using input redirection

     Linux: ```./bt < t1.txt``` 
     
     Windows: ```bt < t1.txt```
 
  **Note** : Replace the t1.txt with t2.txt, t3.txt, t4.txt or t5.txt , to run the corresponding test case file.

Tested successfully on 

Windows: g++ (MinGW.org GCC-8.2.0-5) 8.2.0 

Ubuntu: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3.0
