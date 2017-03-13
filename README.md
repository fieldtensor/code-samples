Table of Contents
=================

* [A Balanced Binary Search Tree](#bst)
* [A Quaternion Class](#quat)
* [A Matrix Class](#mat)
* [An Octree Implementation](#oc)
* [Ambient Occlusion (Ray Tracing Method)](#ao)
* [3D Water (Plane Waves and Fresnel Effect)](#water)
* [Show Volume Optimization](#shadow)
* [Export Script for 3D Models in Blender](#export)
* [IO Stress Tester for RAID Systems](#io)
* [3D Vector Class in x86-64 Assembler Language](#vec)
* [Unicode Processing (UTF-8 and UTF-16)](#unicode)
* [HTML Parsing (Server Side C++)](#html)
* [Mathematics and Physics Notes](#notes)

<a name="bst"></a>
A Balanced Binary Search Tree 
---------------------------

<p align="center">
<img src="/avl.png" width="50%"/>
</p>

<a href="/AVLTree.hpp">[AVLTree.hpp]</a>

This is a balanced binary search tree which is useful for implementing
key-value maps and sets in C++. The specific flavor of BST in this source file
is Adelson-Velsky & Landis (AVL). As new data is inserted the code rebalances
the tree to keep it shallow and easy to search. The maximum depth of the tree
is log(n+1), where n is the number of nodes. The lookup speed is therefore
O(log(n+1)) in the worst case, which is tremendously better than a linear
search for large n.

<a name="quat"></a>
A Quaternion Class
------------------

<p align="center">
<img src="/quat.png" width="20%"/>
</p>

<a href="/Quat.js">[Quat.js]</a>

Quaternions are somewhat obscure 4 dimensional entities that see little use in
modern mathematics and physics. However, quaternions are able to represent 3D
rotations in a compact and computationally efficient way, and so they are used
pervasively by 3D graphics systems. There is no commercial game engine in
existence that does not include a quaternion implementation.

To to give a quick summary, quaternions are a higher dimensional
generalization of the ordinary (two dimensional) complex numbers. Complex
numbers have one real part and one imaginary part, while a quaternion has one
real part and three imaginary parts. Quaternions follow an especially
complicated multiplication rule, form which they derive all of their
mathematical power. A quaternion is able to represent a 3D rotation in just 4
numbers, which is more compact than a 3x3 matrix with 9 numbers. Moreover, it
is possible to cleanly interpolate between two quaternions, and thus between
two rotational states. This is accomplished by rotating one quaternion into
the other on a four dimensional hypersphere. With a rotation matrix such an
interpolation is impossible without suffering malformed rotations or a large
computational burden.

The file <a href="/Quat.js">Quat.js</a> is my implementation of the
quaternions in JavaScript, for WebGL code. The mathematics in this file is not
copied from textbooks or the internet. I rederive all of it from scratch on
paper to better understand how quaternions work. It happens to also be
difficult to find educational material about quaternions because they are
somewhat obscure and not covered well by the mathematical community. I'm
currently in the process of writing up a long article on the subject for my
website.


<a name="mat"></a>
A Matrix Class
--------------

<p align="center">
<img src="/matrix.png" width="20%"/>
</p>

<a href="/Mat4.js">[Mat4.js]</a>

This file contains my implementation of a 4x4 matrix in JavaScript, for use
with WebGL code. The contents and methods of this class should be familiar to
anyone with graphics programming experience, or experience with linear
algebra.

The most interesting thing about this class is that it is implemented in a way
that will never trigger the JavaScript garbage collector. A stack of spare
matrices is kept around and reused in perpetuity. When a matrix is no longer
needed it is returned to the stack (as opposed to being collected by the
JavaScript GC). For example, when the user multiples two matrices a spare
matrix is taken from a preexisting existing pool and used to store the result.
New matrices can be created but but are always returned the pool instead of
being destroyed.

This is an important technique because it prevents the JavaScript garbage
collector from ever running. In a VR application, for example, every frame has
just 11 milliseconds to render. But when JavaScript's garbage collector runs
it stops the program dead in order to do its cleanup work. This causes several
frames to be dropped by the rendering loop and results in a visual stutter. It
is therefore best to avoid the garbage collector whenever possible.


<a name="oc"></a>
An Octree Implementation
------------------------

<p align="center">
<img src="/octree.png">
</p>

<a href="/Octree.hpp">[Octree.hpp]</a><br>
<a href="/Octree.cpp">[Octree.cpp]</a>

This class implements a common type of eight-way space partitioning called an
Octree. I used this Octree to accelerate the ray tracer in my ambient
occlusion implementation. The naive implementation of the ray tracer would
take about 48 hours to bake occlusions on large models. With the Octree in
place the baking time was cut down to just an hour or two.

The octree works by placing the entire 3D scene into a large cube. This cube
in then split vertically, horizontally, and depth wise into 8 smaller cubes.
Each of these smaller cubes is then split into 8 yet smaller cubes. These are
then split again into 8 more cubes each, etc... Eventually the process
terminates when the cubes are small enough, or when there are just too many.
The number of cubes increases at the extremely fast exponential rate of 8^n,
where n is the number of subdivisions, so only a few subdivisions are ever
needed.

Once the splitting is complete we take all of the geometry in the scene and
assign it to the cubes, vertex by vertex. The ray tracer can then use this
whole structure as a kind of spatial index. When doing a ray trace we first
check which cubes a ray intersects. Then we gather up the geometry from those
cubes and perform further intersection test. The geometry that lives in cubes
which the ray does not intersect is never considered, and in this way we gain
a tremendous performance boost.


<a name="ao"></a>
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

<a href="/ao-bake.cpp">[ao-bake.cpp]</a>

This code implements pre-computed ambient occlusion to enhance the realism of
3D scenes in OpenGL. A custom ray tracer is used to detect the nooks and
crannies of 3D models, and those nooks and crannies are then shaded more
darkly than the surrounding geometry.

For example, consider a groove in a screw. If we try to shoot a random ray out
of a screw's groove it probably won't escape to the outside world. It will
more likely hit the sides of the groove. When we detect this situation in our
ray tracer we take it to mean that less light should penetrate into the
groove, because it is occluded by its surrounding walls. The groove is then
given less ambient light from within the system's fragment programs and
appears darker on screen.

The darkening of nooks and crannies greatly enhances the realism of a 3D
scene. The images above show an example. The first image shows a rocky outcrop
without ambient occlusion. The second image shows that same outcrop after it
has been passed through ao-bake.cpp. Notice how the shading on the first
images look flat and homogeneous, while the shading on the second image look
truly three dimensional and alive. 


<a name="water"></a>
3D Water (Plane Waves and Fresnel Effect)
-----------------------------------------

<a href="/water.vs">[water.vs]</a>

This vertex shader simulates a wavy ocean surface for a 3D beach scene. Eight
plane waves of different amplitude and frequency are superimposed to give the
illusion of waves chaotically lapping around on the ocean's surfaces.

This shader also implements another important aspect of water called the
Fresnel effect. The surface of water appears transparent when you look
straight down at it from on high, but appears reflective like a mirror when
seen at an obtuse angle.

For example, consider standing in a lake. If you look far away you will be
able to see the clouds and nearby trees reflected in the lake's surface. Yet
if you look straight down you'll see your feet through the surface of the
water, and not your own reflection. The fact that water can be transparent or
reflective under different conditions is due to the way electromagnetic waves
(photons) behave when crossing the boundary between air the water. This whole
thing is called the Fresnel effect. The shader takes it into account with the
following lines of vector math:

    // The R0 refecltivity constant is determined by empircal expimrents with
    // water. The value given here is taken from optics references.
    
    const float R0 = 0.625;
    
    float omc = 1.0 - max(0.0, dot(toEye, norm));
    
    // Compute R, the reflection coefficient.
    float R = R0 + (1.0 - R0)omcomcomcomcomc;
    
    // Compute T, the reflection coefficient.
    float T = 1.0 - R;


<a name="shadow"></a>
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

<a href="/shadow.vs">[shadow.vs]</a>

This source file is the vertex shader for an implemenation of shadow volumes.
Most of the code for implementing shadow volumes goes into extracting
edge-to-face connectivity information from the 3D model file and then using
that information at runtime to extrude out the shadow volumes. The vertex
shader component of a shadow volume implementation is itself very simple. If
you open the file you'll see that there isn't much to look at.

However, I was able to come up with an optimization for this vertex shader
which reduced the rendering time of shadows on our project from 4.2 ms to just
0.3 ms. At 60 FPS this is a drop from 25% of a single frame to just 2% of a
single frame, which is dramatic. The performance improvement was so large and
surprising that I benchmarked it multiple times and went over the timing code
with a fine toothed comb to confirm that the result was real.


<a name="export"></a>
Export Script for 3D Models in Blender 
--------------------------------------

<a href="/blender-export.py">[blender-export.py]</a>

This is a python script which exports model files from Blender into a custom
file format suitable for our WebGL project. The project had to run on mobile
phones over slow cellular connections, so the model file format was
implemented in binary and highly optimized to reduce file size. Examples of
optimizations included: 

* Using RGB color triplets composed of 3 unsigned bytes, as opposed RGBA color
quadruplets composed of 4 floats.

* Using a half-float polar representation for surface normals to trim their size
to just 8 bytes each, down from 24 bytes each. Surface normals could have even
been theoretically eliminated from the file format altogether at the expense
of slightly longer load times.

The project ran out of funding and we were not able to optimized the file
format as much as desired, just for time and budget reasons. The loading time
nonetheless came out acceptable.


<a name="io"></a>
IO Stress Tester for RAID Systems
---------------------------------

<a href="/cio.cpp">[cio.cpp]</a>

While at Columbia I did a small amount of research work on the Large Hadron
Collider. While on that project I also helped install and manage a small
distributed computing cluster composed of 10 rack mounted servers and a RAID
controller with 50 terabytes of capacity.

At one point we experienced performance bottlenecks on the RAID system. We
suspected that they were due to inefficiencies in the IO of some third party
software from CERN. I wrote a small program, cio.cpp, to stress test the RAID
system and determine it's maximum IO capacity. We needed to write a custom
stress test program because we suspected that our issues were due to the
incorrect uses of the POSIX fsync() function. We needed to mimic that
incorrect usage to compare the performance profiles.


<a name="vec"></a>
3D Vector Class in x86-64 Assembly Language
--------------------------------------------

<a href="/Vec3.hpp">[Vec3.hpp]</a><br>
<a href="/Vec3D-x86-64-sysv.s">[Vec3D-x86-64-sysv.s]</a>

This is a 3D vector class for use in C++. It includes dot produces, cross
products, vector addition, and all of that other usual stuff. The file <a
href="/Vec3.hpp">Vec3.hpp</a> contains an implementation of the class in C++,
while the file <a href="/Vec3D-x86-64-sysv.s">Vec3D-x86-64-sysv.s</a> contains
an implementation of the very same class in pure x86-64 assembler language.

I wrote an implementation in assembler because I wanted to learn how to use
Intel's SIMD instructions. The SIMD op codes are not normally accessible from
C, and they allow for the parallel computations of vector dot products in the
same batch of CPU clock cycles. For numerically intensive code the SIMD can
provide tremendous performance boosts. This class does not actually make use
of the full power of the instruciton set, but it did teach me how to use it
general.


<a name="unicode"></a>
Unicode Processing (UTF-8 and UTF-16)
-------------------------------------

<a href="/Unicode.hpp">[Unicode.hpp]</a><br>
<a href="/Unicode.cpp">[Unicode.cpp]</a>

These files contain collections of functions which encode and decode Unicode
in UTF-8, UTF-16, or UTF-32. UTF-8 and UTF-16 encode Unicode characters in the
range (0x0000000000, 0x10FFFFFFFF) by packing them into streams of octets or
words. Some bits of every byte are used for encoding metadata about the
encoding while other bits are used for encoding the character's actual value.
These functions do bit twiddling to extract the metadata, unpacking the
character payload, and deliver a stream of Unicode characters to the caller.
There are also functions for the encoding process.


<a name="html"></a>
HTML Parsing (Server Side C++)
-------------------------------------

<a href="/HTMLParser.hpp">[HTMLParser.hpp]</a><br>
<a href="/HTMLParser.cpp">[HTMLParser.cpp]</a>

My <a href="http://patrick-rutkowski.com">personal website</a> is written in a
C++ based web framework which I created myself. At the core of the framework
is the idea that server-side construction of HTML should be done by using a
DOM tree (which is then text-ified before output the client). This is in
contrast to the textual cut-and-paste approach used by may Perl or Python
based systems, and PHP. Having the HTML represented as a searchable and
re-arrangeable DOM tree makes many programming tasks much more intuitive.

The HTML parser in these files walks through HTML source code and delivers a
stream of tags and attributes to the caller. The API is in the spirit of the
Expat XML parser. Other code not listed here uses this parser to build a
DOM-like tree of tags and text. The parser can also handle a custom macro
language, as well as some special syntax related to maintaining professional
grade typesetting quality, and some more special syntax related to rendering
mathematics.


<a name="notes"></a>
Mathematics and Physics Notes
-----------------------------

These are PDFs of some of my notes on mathematics and physics (along with
their source files in TeX). Whenever I learn a new piece of mathematics or
physics I find it useful to type out textbook style notes as if trying to
explain the subject to a new student. In the process of trying to teach
something I often realize that I don't understand it as well as I thought I
did. In this way I uncover the gaps in my knowledge and eventually come to a
more complete understanding. 

These notes do not represent the totality of what I know about
mathematics and physics. They're just a cherry picked selection of a few
subjects that I've had time to write about. I tried to keep the selection
trimmed down to things which are relevant to graphics programming, either
directly or tangentially. I left out completely unrelated subjects like
quantum mechanics or general relativity.

I don't necessarily proof read these since they're just for my own future
reference, and the important part is always the mathematics itself. There
maybe a good deal of type-o's and grammatical errors.

<a href="/rotation-about-a-vector.pdf">Rotation about a Vector</a> &mdash;
<a href="/rotation-about-a-vector.tex">(TeX Source)</a><br>

<a href="/euler-lagrange-equation.pdf">Euler Lagrange Equation</a> &mdash;
<a href="/euler-lagrange-equation.tex">(TeX Source)</a><br>

<a href="/double-pendulum.pdf">Double Pendulum</a> &mdash;
<a href="/double-pendulum.tex">(TeX Source)</a><br>

<a href="/euler-angles.pdf">Euler Angles</a> &mdash;
<a href="/euler-angles.tex">(TeX Source)</a><br>

<a href="/energy-wells.pdf">Energy Wells</a> &mdash;
<a href="/energy-wells.tex">(TeX Source)</a><br>

<a href="/fourier-series.pdf">Fourier Series</a> &mdash;
<a href="/fourier-series.tex">(TeX Source)</a><br>

<a href="/taylor-series.pdf">Taylor Series</a> &mdash;
<a href="/taylor-series.tex">(TeX Source)</a><br>

<a href="/dot-product.pdf">Dot Product</a> &mdash;
<a href="/dot-product.tex">(TeX Source)</a><br>

<a href="/cross-product.pdf">Cross Products</a> &mdash;
<a href="/cross-product.tex">(TeX Source)</a><br>

<a href="/angular-acceleration.pdf">Angular Acceleration</a> &mdash;
<a href="/angular-acceleration.tex">(TeX Source)</a><br>

<a href="/angular-velocity-vector.pdf">Angular Velocity Vector</a> &mdash;
<a href="/angular-velocity-vector.tex">(TeX Source)</a><br>

<a href="/perspective-projection.pdf">Perspective Projection</a> &mdash;
<a href="/perspective-projection.tex">(TeX Source)</a><br>

<a href="/polar-vector-derivatives.pdf">Polar Vector Derivatives</a> &mdash;
<a href="/polar-vector-derivatives.tex">(TeX Source)</a><br>

<a href="/rigid-body-properties.pdf">Rigid Body Properties</a> &mdash;
<a href="/rigid-body-properties.tex">(TeX Source)</a><br>

<a href="/spherical-coordinates.pdf">Spherical Coordinates</a> &mdash;
<a href="/spherical-coordinates.tex">(TeX Source)</a><br>

<a href="/tensor-of-inertia.pdf">Tensor of Inertia</a> &mdash;
<a href="/tensor-of-inertia.tex">(TeX Source)</a><br>

<a href="/transport-theorem.pdf">Transport Theorem</a> &mdash;
<a href="/transport-theorem.tex">(TeX Source)</a><br>

<a href="/vector-curl.pdf">Vector Curl</a> &mdash;
<a href="/vector-curl.tex">(TeX Source)</a><br>

<a href="/vector-projection-operators.pdf">Vector Projection Operators</a> &mdash;
<a href="/vector-projection-operators.tex">(TeX Source)</a><br>

<a href="/surface-integrals.pdf">Surface Integrals</a> &mdash;
<a href="/surface-integrals.tex">(TeX Source)</a><br>

<a href="/volume-integrals.pdf">Volume Integrals</a> &mdash;
<a href="/volume-integrals.tex">(TeX Source)</a><br>

<a href="/cosh-and-sinh.pdf">Cosh and Sinh</a> &mdash;
<a href="/cosh-and-sinh.tex">(TeX Source)</a><br>

<a href="/derivative-of-arctan.pdf">Derivative of Arctan</a> &mdash;
<a href="/derivative-of-arctan.tex">(TeX Source)</a><br>

<a href="/derivatives-of-sin-cos-arcsin-and-arccos.pdf">Derivatives of sin, cos, arcsin, and arccos</a> &mdash;
<a href="/derivatives-of-sin-cos-arcsin-and-arccos.tex">(TeX Source)</a><br>
