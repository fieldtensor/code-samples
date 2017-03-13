Table of Contents
=================

Balanced Binary Search Tree 
---------------------------

**[AVLTree.hpp]**

This is a balanced binary search tree, useful for implementing key-value maps
and sets in C++. The specific flavor of BST in this source file is
Adelson-Velsky & Landis (AVL). As data is inserted into the tree the code
rebalances nodes to keep it shallow and easy to search. The maximum depth of
the tree is log(n+1), where n is the number of nodes. The lookup speed is
therefore O(log(n+1)) in the worst case, which is tremendously better than a
linear search for large n.
