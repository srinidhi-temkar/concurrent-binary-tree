# concurrent-binary-tree
Implementation of a concurrent version of a binary tree data structure

# About test cases
* **t1.txt** file consisting of test cases which handle basic operations of the binary tree, this text case file gives importance on reflecting the appropriate working of all the basic operations in binary tree.

* **t2.txt** file consisting of test cases which handle edge cases and reflecting the importance on how the mutex locks handle to prevent writing into the buffer when another read-thread is printing the tree.

* **t3.txt** file consisting of test cases to reflect the usage of read-write locks.

* **t4.txt** test case handles large number of insertion and deletion operations.

* **t5.txt** file includes large number of operations performed in random.

# Compiling the code
* **Comiling**

      g++ -o bin_tree bt.cpp -lpthread
      
* **Running the executable** using input redirection

      ./bin_tree < t1.txt
 
  **Note** : Replace the t1.txt with t2.txt, t3.txt, t4.txt or t5.txt , to run the corresponding test case file.
