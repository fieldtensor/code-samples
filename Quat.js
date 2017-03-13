"use strict";

RE.quatStack = [];
RE.quatStackTop = 0;

function
REMakeQuat(r, x, y, z)
{
    while( RE.quatStackTop >= RE.quatStack.length )
        RE.quatStack.push(new REQuat(r, x, y, z));

    var q = RE.quatStack[RE.quatStackTop++];

    q.r = r;
    q.x = x;
    q.y = y;
    q.z = z;

    return q;
}

function
RENewQuat()
{
    return new REQuat(0, 0, 0, 0);
}

function
REQuat(r, x, y, z)
{
    this.r = r;
    this.x = x;
    this.y = y;
    this.z = z;
}

function
REQuatSet(q)
{
    this.r = q.r;
    this.x = q.x;
    this.y = q.y;
    this.z = q.z;
}

function
REQuatSet4(r, x, y, z)
{
    this.r = r;
    this.x = x;
    this.y = y;
    this.z = z;
}

function
REQuatDot(q)
{
    return this.r * q.r +
           this.x * q.x +
           this.y * q.y +
           this.z * q.z;
}

function
REQuatRotationMatrix()
{
    var r = this.r;
    var x = this.x;
    var y = this.y;
    var z = this.z;

    return REMakeMat4(
        r*r + x*x - y*y - z*z, 2*x*y - 2*r*z, 2*x*z + 2*r*y, 0,
        2*x*y + 2*r*z, r*r - x*x + y*y - z*z, 2*y*z - 2*r*x, 0,
        2*x*z - 2*r*y, 2*r*x + 2*y*z, r*r - x*x - y*y + z*z, 0,
        0, 0, 0, 1);
}

function
REQuatMakeWithRotationMatrix(m)
{
    /*
    The mathematics here is easy to derive. Notice that the quaternionic
    rotation matrix has pairs of diagonally opposite values which look very
    similar. There are 3 such pairs. These pairs can each be treated as
    systems of equations and solved solved for r*x, r*y, and r*z. We can then
    find r by doing some some algebra on r^2 + x^2 + y^2 + z^2 = 1.
    */

    var rx = (m.m[9] - m.m[6]) / 4;
    var ry = (m.m[2] - m.m[8]) / 4;
    var rz = (m.m[4] - m.m[1]) / 4;

    var r = Math.sqrt(1 + m.m[0] + m.m[5] + m.m[10]) / 2;

    return REMakeQuat(r, rx / r, ry / r, rz / r);
}

function
REQuatAdd(q)
{
    return REMakeQuat(
        this.r + q.r,
        this.x + q.x,
        this.y + q.y,
        this.z + q.z);
}

function
REQuatAdd4(r, x, y, z)
{
    return REMakeQuat(
        this.r + r,
        this.x + x,
        this.y + y,
        this.z + z);
}

function
REQuatSub(q)
{
    return REMakeQuat(
        this.r - q.r,
        this.x - q.x,
        this.y - q.y,
        this.z - q.z);
}

function
REQuatSub4(r, x, y, z)
{
    return REMakeQuat(
        this.r - r,
        this.x - x,
        this.y - y,
        this.z - z);
}

function
REQuatMult(q)
{
    var a = this;

    return REMakeQuat(
        a.r * q.r - (a.x*q.x + a.y*q.y + a.z*q.z),
        a.r * q.x + q.r * a.x + (a.y*q.z - a.z*q.y),
        a.r * q.y + q.r * a.y + (a.z*q.x - a.x*q.z),
        a.r * q.z + q.r * a.z + (a.x*q.y - a.y*q.x));
}

function
REQuatMult4(r, x, y, z)
{
    var a = this;

    return REMakeQuat(
        a.r * r - (a.x*x + a.y*y + a.z*z),
        a.r * x + r * a.x + (a.y*z - a.z*y),
        a.r * y + r * a.y + (a.z*x - a.x*z),
        a.r * z + r * a.z + (a.x*y - a.y*x));
}

function
REQuatMultScalar(r)
{
    return REMakeQuat(
        r * this.r,
        r * this.x,
        r * this.y,
        r * this.z);
}

function
REQuatDivScalar(r)
{
    return REMakeQuat(
        this.r / r,
        this.x / r,
        this.y / r,
        this.z / r);
}

function
REQuatSlerp(a, b, t)
{
    var dot = a.dot(b);

    if( dot < 0 )
    {
        b = b.multScalar(-1);
        dot = -dot;
    }

    var angle = Math.acos(dot);

    var s = Math.sin(angle);
    if( s == 0.0 )
        return a;

    var s1 = Math.sin((1 - t) * angle);
    var s2 = Math.sin(t * angle);


    return (
        a.multScalar(s1)
        .add(
        b.multScalar(s2))
    ).divScalar(s);
}

REQuat.prototype.set = REQuatSet;
REQuat.prototype.set4 = REQuatSet4;
REQuat.prototype.dot = REQuatDot;
REQuat.prototype.rotationMatrix = REQuatRotationMatrix;
REQuat.prototype.add = REQuatAdd;
REQuat.prototype.add4 = REQuatAdd4;
REQuat.prototype.sub = REQuatSub;
REQuat.prototype.sub4 = REQuatSub4;
REQuat.prototype.mult = REQuatMult;
REQuat.prototype.mult4 = REQuatMult4;
REQuat.prototype.multScalar = REQuatMultScalar;
REQuat.prototype.divScalar = REQuatDivScalar;
