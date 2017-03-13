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

<p align="center">
<img src="/octree.png">
</p>

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

Ambient Occlusion (Ray Tracing Method)
--------------------------------------

<p align="center">
<img src="/rocks-ao-off.png" width="45%"/>
<img src="/rocks-ao-on.png" width="45%"/>
<br>
<i>(Left)</i> Ambient Occlusion Off (nooks not shaded)
&mdash;
<i>(Right)</i> Ambient Occlusion On (nooks shaded)
</p>

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
of a 3D scene. For example, the images above show a rocky outcrop. The first
image shows the 3D model without ambient occlusion. The second image shows
that same model after it has been passed through ao-bake.cpp. Notice how the
shading on the first images look flat and homogeneous, while the shading on
the second image look truly three dimensional and alive. 


3D Water (Plane Waves and Fresnel Effect)
-----------------------------------------

**[water.vs]**

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


Show Volume Optimization
------------------------

<p align="center">
<img src="/shadow.png" width="45%"/>
<img src="/shadow-volume.png" width="45%"/>
<br>
<i>(Left)</i> Shadows in final rendering
&mdash;
<i>(Right)</i> Debug visualization of shadow volumes
</p>

**[shadow.vs]**

This source file is the vertex shader component of a shadow volumes
implementation. Most of the code for implementing shadow volumes goes into
extracting edge-face connectivity information from the 3D model file and then
using that information at runtime to extrude out the shadow volumes. The
vertex shader component for shadow volumes is itself very simple. If you open
the file you'll see that there isn't much to look at.

However, I was able to come up with an optimization for this vertex shader
which reduced the rendering time of shadows on our project from 4.2 ms to just
0.3 ms. At 60 FPS this is a drop from 25% of a single frame to just 2% of a
single frame, which is dramatic. The performance improvement was so large and
surprising that I benchmarked it multiple times and went over the timing code
with a fine toothed comb to confirm the result.


Export Script for 3D Models in Blender 
--------------------------------------

**<a href="/blender-export.py">[blender-export.py]</a>**

This is a pythons script which exports model files from Blender into a custom
file format suitable for our WebGL project. The project had to run on mobile
phones over slow cellular connections, so the model file format was
implemented in binary and highly optimized to reduce file size. Examples of
optimized included: 

* Using RGB color triplets composed of 3 unsigned bytes, as opposed RGBA color
quadruplets composed of 4 floating point. 

* Using a half-float polar representation for surface normals to trim their size
to just 8 bytes each, down from 24 bytes each. Surface normals could have even
been theoretically eliminated from the file format altogether, at the expense
of slightly longer load times.

The project ran out of funding and we were not able to optimized the file
format as much as desired, just for time and budget reasons. The loading time
nonetheless came out acceptable.

IO Stress Tester for RAID Systems
---------------------------------

**<a href="/cio.cpp">[cio.cpp]</a>**

While at Columbia I did a small amount of research work on the Large Hadron
Collider. While on that project I also helped install and manage a small
distributed computing cluster with 10 high powered DELL servers, and an
attached RAID controller with 50 terabytes of capacity.

At one point we experienced performance bottlenecks on the RAID system. We
suspected that they were due to inefficiencies in the IO code some of third
party software from CERN. I wrote a small program, cio.cpp, to stress test the
RAID system and determine it's maximum IO capacity. We needed to write a
custom stress test program instead of using an existing one because we
suspected that our issues were due to incorrect uses of the POSIX fsync()
function. We needed to mimic that incorrect usage and compare against the
results.


3D Vector Class in x86-64 Assembler Language
--------------------------------------------

**<a href="/Vec3.hpp">[Vec3.hpp]</a>**
**<a href="/Vec3D-x86-64-sysv.s">[Vec3D-x86-64-sysv.s]</a>**

This is a 3D vector class for use in C++. It includes dot produces, cross
products, vector addition, and all of that other usual stuff. The file <a
href="/Vec3.hpp">Vec3.hpp</a> contains an implementation of the class in C++,
while the file <a href="/Vec3D-x86-64-sysv.s">Vec3D-x86-64-sysv.s</a> contains
an implementation of the very same class in pure x86-64 assembler language.

I wrote an implementation in assembler because I wanted to learn how to use
Intel's SIMD instructions. The SIMD op codes are not normally accessible from
C, and they allow for the parallel computations of vector dot products in the
same batch of CPU clock cycles. For numerically intensive code the SIMD op
codes can provide tremendous performance boosts. This class does not actually
make use of the full power of SIMD, but it did teach me how to use it general.


Unicode Processing (UTF-8 and UTF-16)
-------------------------------------

**<a href="/Unicode.hpp">[Unicode.hpp]</a>**
**<a href="/Unicode.cpp">[Unicode.cpp]</a>**

These files contain collections of functions which encode and decode Unicode
in UTF-8, UTF-16, or UTF-32. UTF-8 and UTF-16 encode Unicode characters in the
range (0x0000000000, 0x10FFFFFFFF) by packing them into streams of octets or
words. Some bits of every byte are used for encoding metadata about the
encoding while other bits are used for encoding the character's actual value.
These functions do bit twiddling to extract the metadata, unpacking the
character payload, and deliver a simple stream of Unicode characters to the
caller. There are also functions for the reverse encoding process.


HTML Parsing (Server Side C++)
-------------------------------------

**<a href="/HTMLParser.hpp">[HTMLParser.hpp]</a>**
**<a href="/HTMLParser.cpp">[HTMLParser.cpp]</a>**

My <a href="http://patrick-rutkowski.com">personal website</a> is written in a
C++ based web framework which I created myself. At the core of the framework
is the idea that HTML should be constructed as a DOM tree and then text-ified
before final output the client. This is in contrast to the text base
cut-and-paste approach used by may Perl/Python based systems, and PHP. Having
the HTML represented as a searchable and re-arrangeable DOM tree makes many
programming tasks much more intuitive.

The HTML parser in these files walks through HTML source code and delivers a
stream of tags and attributes to the caller. The API is in the spirit of the
Expat XML parser. Other code not listed here uses this parser to build a
DOM-like tree of tags and text. The parser can also handle a custom macro
language used to facilitate the creation of dynamic content, as well as some
special syntax related to maintaining professional grade typesetting quality,
and some more special syntax related to rendering mathematics.


Mathematics and Physics Notes
-----------------------------

**Angular Acceleration**
<a href="/HTMLParser.hpp">(PDF)</a> &mdash;
<a href="/HTMLParser.hpp">(TeX Source)</a>

./angular-acceleration.pdf
./angular-velocity-vector.pdf
./cosh-and-sinh.pdf
./cross-product.pdf
./derivative-of-arctan.pdf
./derivatives-of-sin-cos-arcsin-and-arccos.pdf
./dot-product.pdf
./double-pendulum.pdf
./energy-wells.pdf
./euler-angles.pdf
./euler-lagrange-equation.pdf
./fourier-series.pdf
./perspective-projection.pdf
./polar-vector-derivatives.pdf
./rigid-body-properties.pdf
./rotation-about-a-vector.pdf
./spherical-coordinates.pdf
./surface-integrals.pdf
./taylor-series.pdf
./tensor-of-inertia.pdf
./transport-theorem.pdf
./vector-curl.pdf
./vector-projection-operators.pdf
./volume-integrals.pdf

