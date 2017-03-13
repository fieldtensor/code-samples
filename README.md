Table of Contents
=================

A Balanced Binary Search Tree 
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


An Octree Implementation
------------------------

**[Octree.hpp]**
<br>
**[Octree.cpp]**

This class implements a common type of eight-way 3D space partitioning called
an Octree. I used this Octree to accelerate the ray tracer in my ambient
occlusion implementation. The naive implementation of the ray tracer would
take about 48 hours to bake occlusions on large models. With the Octree in
place the baking time was cut down to just an hour or two.

The octree works by placing the entire 3D scene into a large cube. This cube
in then split vertically, horizontally, and depth wise into 8 smaller cubes.
Each of this smaller cubes is then split into 8 yet smaller cubes. These are
then split again into 8 more cubes each, etc... Eventually the process
terminates when the cubes are small enough, or they are just too many (the
number of cubes increases at the extremely fast exponential rate of 8^n, where
n is the number of subdivisions).

Once the splitting geometry in the scene is assigned to each of the cubes,
vertex by vertex. The ray tracer can then use this whole structure as a kind
of spatial index. We first check which cubes a ray intersects, and then we
gather up the geometry from those cubes and intersect the ray further with
that geometry. The geometry that lives in cubes which the ray does not
intersect is never considered, and this is where the tremendous performance
acceleration comes from.

<p align="center">
<img src="/octree.png">
</p>


Ambient Occlusion (Ray Tracing Method)
--------------------------------------

**[ao-bake.cpp]**

This code implements pre-computed ambient occlusion to enhance the realism of
a 3D scene in OpenGL. A custom ray tracer is used to detect the nooks and
crannies of 3D models. For example, consider a groove in a screw. If we try to
shoot a random ray out of a screw's groove it probably won't escape to the
outside world. It will more likely hit the sides of the groove. We can detect
this, and we take it to mean that less light should penetrate into the groove.
The groove is given less ambient light from within the system's fragment
programs, and appears more dark on screen.

The darkening of nooks and crannies in this way greatly enhances the realism
of a 3D scene, as can be seen below. The first image shows a model without
ambient occlusion. The second image shows that same model after a pass through
ao-bake.cpp. 

<p align="center">
<img src="/rocks-ao-off.png" width="85%"/>
<img src="/rocks-ao-on.png" width="45%"/>
</p>

3D Water (Plane Waves and Fresnel Effect)
-----------------------------------------

This vertex shader simulates a wavy ocean surface for a 3D beach scene. Eight
plane waves of different amplitude and frequency are superimposed to give the
illusion of waves chaotically lapping around on the ocean's surfaces.

This shader also implements another important aspect of water called the
Fresnel effect. The surface of water appears transparent when you look
straight down into it from on high, but appears reflective like a mirror when
you look at it from the side an obtuse angle.

For example, consider standing in a lake. If you look far away you will be
able to see nearby trees reflected in the lake's surface. Yet if you look
straight down at your feet you won't see your reflections, but rather you'll
see through to the lake's muddy bottom. The fact that water is transpoart or
reflective depending on the viewing angle is due to the way electromagnetic
waves (photons) behave at the boundary between the air the water. This is
called the Fresnel effect. The shader takes it into account with the following
lines of simple vector math:

    // The R0 refecltivity constant is determined by empircal expimrents with
    // water. The value given here is taken from optics references.
    
    const float R0 = 0.625;
    
    float omc = 1.0 - max(0.0, dot(toEye, norm));
    
    // Compute R, the reflection coefficient.
    float R = R0 + (1.0 - R0)*omc*omc*omc*omc*omc;
    
    // Compute T, the reflection coefficient.
    float T = 1.0 - R;
