Table of Contents
=================

Balanced Binary Search Tree 
---------------------------

**[AVLTree.hpp]**

This is a balanced binary search tree useful for implementing key-value maps
and sets in C++. The specific flavor of BST in this source file is
Adelson-Velsky & Landis (AVL). As new data is inserted the code rebalances the
tree to keep it shallow and easy to search. The maximum depth of the tree is
log(n+1), where n is the number of nodes. The lookup speed is therefore
O(log(n+1)) in the worst case, which is tremendously better than a linear
search for large n.

I also have an interesting mathematical proof of the correctness of the
balancing procedure somewhere in my hand written notes, but I can't find it at
the moment.

A Quaternion Class
------------------

**[Quat.js]**

Quaternions obscure 4 dimensional mathematical entities that see little use in
academic mathematics and physics. However, quaternions are able to represent
3D rotations in a compact and computationally efficient way, and so they are
used pervasively by 3D graphics systems. There is no commercial game engine in
existence that does not include a quaternion implementation.

To to give a quick summary, Quaternions are a four dimensional generalization
of the ordinary two dimensional complex numbers. A complex numbers have one
real part and one imaginary part, while a quaternion has one real part and
three complex parts. Quaternions follow an especially complicated
multiplication rule, form which they derive all of their mathematical and
computational power. A quaternion is able to represent a 3D rotation in just 4
numbers, which is more compact than a 3x3 matrix with 9 numbers. Moreover, it
is possible to cleanly interpolate between two quaternions, and thus between
two rotational states. This is accomplished by rotating one quaternion into
the other on a four dimensional hypersphere. With a rotation matrix such an
interpolation is impossible to do without suffering malformed rotations or a
large computational burden on the FPU. 

The file Quat.js is my implementation of the Quaternions in JavaScript, for
WebGL code. The mathematics in this file is not copied from textbooks or the
internet. I rederive all of it from scratch on paper to better understand how
quaternions work. It happens to also be difficult to find detailed
explanations for how quaternions work, because they are somewhat obscure and
not covered well by the mathematical community. I'm currently in the process
of writing up a long article on the subject for my website.


A Matrix Class
--------------

**[Mat4.js]**

This file contains my implementation of a 4x4 matrix in JavaScript, for use
with WebGL code. The contents and methods of this class should be familiar to
anyone with even rudimentary graphics programming experience, or experience
with linear algebra.

The most interesting thing about this class is that it is implemented in a way
that will never trigger the JavaScript garbage collector. A stack of matrices
is kept around and re-used in perpetuity. For example, when the user multiples
two matrices a spare matrix is taken from a preexisting existing pool and used
to store the result. Matrices are created but never destroyed.

This is an important technique because it prevents the JavaScript garbage
collector from ever running. In a VR application, for example, every frame has
just 11 milliseconds to render. But when JavaScript's garbage collector runs
it stops the program dead in order to do its cleanup work. This causes several
frames to be dropped by the rendering loop and results in a visual stutter.
When programming 3D applications in JavaScript it is tremendously important to
keep all objects around and code in a GC-free style to prevent such
stuttering.

We can, of course, free large amounts of data in many other parts of a WebGL
application. We don't want to leak memory to infinity in general. But
creating, destroying, and re-creating several thousand mathematical
temporaries 90 times per second would be a big mistake. 


A Matrix Class
--------------

**[Octree.hpp]**
<br>
**[Octree.cpp]**


