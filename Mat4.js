"use strict";

RE.mat4Stack = [];
RE.mat4StackTop = 0;

function
REMakeMat4(
    m11, m12, m13, m14,
    m21, m22, m23, m24,
    m31, m32, m33, m34,
    m41, m42, m43, m44)
{
    while( RE.mat4StackTop >= RE.mat4Stack.length )
        RE.mat4Stack.push(new REMat4(
            m11, m12, m13, m14,
            m21, m22, m23, m24,
            m31, m32, m33, m34,
            m41, m42, m43, m44));

    var m = RE.mat4Stack[RE.mat4StackTop++];

    return m.set16(
        m11, m12, m13, m14,
        m21, m22, m23, m24,
        m31, m32, m33, m34,
        m41, m42, m43, m44);
}

function
REAllocMat4()
{
    while( RE.mat4StackTop >= RE.mat4Stack.length )
        RE.mat4Stack.push(new REMat4(
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0));

    return RE.mat4Stack[RE.mat4StackTop++];
}

function
RENewMat4()
{
    return new REMat4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);
}

function
REMat4(
    m11, m12, m13, m14,
    m21, m22, m23, m24,
    m31, m32, m33, m34,
    m41, m42, m43, m44)
{
    this.m = new Float64Array(16);

    var m = this.m;
    m[0]  = m11; m[1]  = m12; m[2]  = m13; m[3]  = m14;
    m[4]  = m21; m[5]  = m22; m[6]  = m23; m[7]  = m24;
    m[8]  = m31; m[9]  = m32; m[10] = m33; m[11] = m34;
    m[12] = m41; m[13] = m42; m[14] = m43; m[15] = m44;
}

function
REMat4Copy()
{
    var m = this.m;
    return new REMat4(
        m[0],  m[1],  m[2],  m[3],
        m[4],  m[5],  m[6],  m[7],
        m[8],  m[9],  m[10], m[11],
        m[12], m[13], m[14], m[15]);
}

function
REMat4Set(m)
{
    for(var i = 0; i < 16; i++)
        this.m[i] = m.m[i];
    return this;
}

function
REMat4Set16(
    m11, m12, m13, m14,
    m21, m22, m23, m24,
    m31, m32, m33, m34,
    m41, m42, m43, m44)
{
    var m = this.m;
    m[0]  = m11; m[1]  = m12; m[2]  = m13; m[3]  = m14;
    m[4]  = m21; m[5]  = m22; m[6]  = m23; m[7]  = m24;
    m[8]  = m31; m[9]  = m32; m[10] = m33; m[11] = m34;
    m[12] = m41; m[13] = m42; m[14] = m43; m[15] = m44;
    return this;
}

function
REMat4Mult3v(v)
{
    return REMakeVec3(
        this.m[0] * v.x + this.m[1] * v.y + this.m[2]  * v.z + this.m[3],
        this.m[4] * v.x + this.m[5] * v.y + this.m[6]  * v.z + this.m[7],
        this.m[8] * v.x + this.m[9] * v.y + this.m[10] * v.z + this.m[11]);
}

function
REMat4Mult3(x, y, z)
{
    return REMakeVec3(
        this.m[0] * x + this.m[1] * y + this.m[2]  * z + this.m[3],
        this.m[4] * x + this.m[5] * y + this.m[6]  * z + this.m[7],
        this.m[8] * x + this.m[9] * y + this.m[10] * z + this.m[11]);
}

function
REMat4Mult4(x, y, z, w)
{
    var m = this.m;
    
    return REMakeVec3(
        m[0] * x + m[1] * y + m[2]  * z + m[3]  + w * m[4],
        m[4] * x + m[5] * y + m[6]  * z + m[7]  + w * m[8],
        m[8] * x + m[9] * y + m[10] * z + m[11] + w * m[12]);
}

function
REMat4Mult(m)
{
    var a = this.m;
    var b = m.m;

    return REMakeMat4(
        a[0]  * b[0] + a[1]  * b[4] + a[2]  * b[8]  + a[3]  * b[12],
        a[0]  * b[1] + a[1]  * b[5] + a[2]  * b[9]  + a[3]  * b[13],
        a[0]  * b[2] + a[1]  * b[6] + a[2]  * b[10] + a[3]  * b[14],
        a[0]  * b[3] + a[1]  * b[7] + a[2]  * b[11] + a[3]  * b[15],
        a[4]  * b[0] + a[5]  * b[4] + a[6]  * b[8]  + a[7]  * b[12],
        a[4]  * b[1] + a[5]  * b[5] + a[6]  * b[9]  + a[7]  * b[13],
        a[4]  * b[2] + a[5]  * b[6] + a[6]  * b[10] + a[7]  * b[14],
        a[4]  * b[3] + a[5]  * b[7] + a[6]  * b[11] + a[7]  * b[15],
        a[8]  * b[0] + a[9]  * b[4] + a[10] * b[8]  + a[11] * b[12],
        a[8]  * b[1] + a[9]  * b[5] + a[10] * b[9]  + a[11] * b[13],
        a[8]  * b[2] + a[9]  * b[6] + a[10] * b[10] + a[11] * b[14],
        a[8]  * b[3] + a[9]  * b[7] + a[10] * b[11] + a[11] * b[15],
        a[12] * b[0] + a[13] * b[4] + a[14] * b[8]  + a[15] * b[12],
        a[12] * b[1] + a[13] * b[5] + a[14] * b[9]  + a[15] * b[13],
        a[12] * b[2] + a[13] * b[6] + a[14] * b[10] + a[15] * b[14],
        a[12] * b[3] + a[13] * b[7] + a[14] * b[11] + a[15] * b[15]);
}

function
REMat4Mult16(
    m11, m12, m13, m14,
    m21, m22, m23, m24,
    m31, m32, m33, m34,
    m41, m42, m43, m44)
{
    var a = this.m;

    return REMakeMat4(
        a[0]  * m11 + a[1]  * m21 + a[2]  * m31 + a[3]  * m41,
        a[0]  * m12 + a[1]  * m22 + a[2]  * m32 + a[3]  * m42,
        a[0]  * m13 + a[1]  * m23 + a[2]  * m33 + a[3]  * m43,
        a[0]  * m14 + a[1]  * m24 + a[2]  * m34 + a[3]  * m44,
        a[4]  * m11 + a[5]  * m21 + a[6]  * m31 + a[7]  * m41,
        a[4]  * m12 + a[5]  * m22 + a[6]  * m32 + a[7]  * m42,
        a[4]  * m13 + a[5]  * m23 + a[6]  * m33 + a[7]  * m43,
        a[4]  * m14 + a[5]  * m24 + a[6]  * m34 + a[7]  * m44,
        a[8]  * m11 + a[9]  * m21 + a[10] * m31 + a[11] * m41,
        a[8]  * m12 + a[9]  * m22 + a[10] * m32 + a[11] * m42,
        a[8]  * m13 + a[9]  * m23 + a[10] * m33 + a[11] * m43,
        a[8]  * m14 + a[9]  * m24 + a[10] * m34 + a[11] * m44,
        a[12] * m11 + a[13] * m21 + a[14] * m31 + a[15] * m41,
        a[12] * m12 + a[13] * m22 + a[14] * m32 + a[15] * m42,
        a[12] * m13 + a[13] * m23 + a[14] * m33 + a[15] * m43,
        a[12] * m14 + a[13] * m24 + a[14] * m34 + a[15] * m44);
}

function
REMat4Decompose(trans, rot, scale)
{
    var m = this.m;

    var lenX = Math.sqrt(m[0]*m[0] + m[4]*m[4] + m[8]*m[8]);
    var lenY = Math.sqrt(m[1]*m[1] + m[5]*m[5] + m[9]*m[9]);
    var lenZ = Math.sqrt(m[2]*m[2] + m[6]*m[6] + m[10]*m[10]);

    scale.set16(
        lenX, 0, 0, 0,
        0, lenY, 0, 0,
        0, 0, lenZ, 0,
        0, 0, 0, 1);

    rot.set16(
        m[0] / lenX, m[1] / lenY, m[2] / lenZ, 0,
        m[4] / lenX, m[5] / lenY, m[6] / lenZ, 0,
        m[8] / lenX, m[9] / lenY, m[10] / lenZ, 0,
        0, 0, 0, 1);

    trans.set16(
        1, 0, 0, m[3],
        0, 1, 0, m[7],
        0, 0, 1, m[11],
        0, 0, 0, 1);
}

function
REMat4Tx()
{
    return this.m[3];
}

function
REMat4Ty()
{
    return this.m[7];
}

function
REMat4Tz()
{
    return this.m[11];
}

function
REMat4ShiftVec()
{
    return REMakeVec3(this.tx(), this.ty(), this.tz());
}

function
REMat4Transposed()
{
    var m = this.m;

    return REMakeMat4(
        m[0], m[4], m[8],  m[12],
        m[1], m[5], m[9],  m[13],
        m[2], m[6], m[10], m[14],
        m[3], m[7], m[11], m[15]);
}

function
REMat4Transpose()
{
    var m = this.m;

    var m11; var m12; var m13; var m14;
    var m21; var m22; var m23; var m24;
    var m31; var m32; var m33; var m34;
    var m41; var m42; var m43; var m44;

    m12 = m[1];  m13 = m[2];  m14 = m[3];
    m21 = m[4];  m23 = m[6];  m24 = m[7];
    m31 = m[8];  m32 = m[9];  m34 = m[11];
    m41 = m[12]; m42 = m[13]; m43 = m[14];

    m[1]  = m21; m[2]  = m31; m[3]  = m41;
    m[4]  = m12; m[6]  = m32; m[7]  = m42;
    m[8]  = m13; m[9]  = m23; m[11] = m43;
    m[12] = m14; m[13] = m24; m[14] = m34;
}

function
REMat4ToString()
{
    var str = "";

    str += this.m[0] + ", ";
    str += this.m[1] + ", ";
    str += this.m[2] + ", ";
    str += this.m[3] + "\n";

    str += this.m[4] + ", ";
    str += this.m[5] + ", ";
    str += this.m[6] + ", ";
    str += this.m[7] + "\n";

    str += this.m[8] + ", ";
    str += this.m[9] + ", ";
    str += this.m[10] + ", ";
    str += this.m[11] + "\n";

    str += this.m[12] + ", ";
    str += this.m[13] + ", ";
    str += this.m[14] + ", ";
    str += this.m[15] + "\n";

    return str;
}

function
REMat4Sync(uniformID)
{
    if( ! RE.mat32 )
        RE.mat32 = new Float32Array(16);

    var a = this.m;
    var b = RE.mat32;

    b[0]  = a[0]; b[1]  = a[4]; b[2]  = a[8];  b[3]  = a[12];
    b[4]  = a[1]; b[5]  = a[5]; b[6]  = a[9];  b[7]  = a[13];
    b[8]  = a[2]; b[9]  = a[6]; b[10] = a[10]; b[11] = a[14];
    b[12] = a[3]; b[13] = a[7]; b[14] = a[11]; b[15] = a[15];

    gl.uniformMatrix4fv(uniformID, gl.FALSE, b);

    //if( gl.getError() != gl.NO_ERROR )
    //    throw "uniformMatrix4fv() failed";
}

function
REMakePerspective(fovy, aspect, near, far)
{
    var cot = 1 / Math.tan(fovy/2);

    if( far == Infinity )
    {
        return REMakeMat4(
            cot / aspect, 0, 0, 0,
            0, cot, 0, 0,
            0, 0, -1, -2*near,
            0, 0, -1, 0);
    }
    else
    {
        return REMakeMat4(
            cot / aspect, 0.0, 0.0, 0.0,
            0.0, cot, 0.0, 0.0,
            0.0, 0.0, (near+far) / (near-far), 2*near*far / (near-far),
            0.0, 0.0, -1.0, 0.0);
    }
}

function
REMakeOrtho(width, aspect, depth)
{
    var w = width;
    var h = w/aspect;
    var d = depth;

    return REMakeMat4(
        2/w, 0, 0, 0,
        0, 2/h, 0, 0,
        0, 0, -2/d, 0,
        0, 0, 0, 1);
}

function
REMakeOrthoExt(l, r, b, t, n, f)
{
    return REMakeMat4(
        2/(r - l), 0, 0, (l+r)/(l-r),
        0, 2/(t - b), 0, (b+t)/(b-t),
        0, 0, 2/(n - f), (n+f)/(n-f),
        0, 0, 0, 1);
}

function
REMakeTranslation4(x, y, z)
{
    return REMakeMat4(
        1, 0, 0, x,
        0, 1, 0, y,
        0, 0, 1, z,
        0, 0, 0, 1);
}

function
REMakeScale4(x, y, z)
{
    return REMakeMat4(
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1);
}

function
REMakeRotation4(angle, x, y, z)
{
    var c = Math.cos(angle);
    var s = Math.sin(angle);

    var omc = 1 - c;

    var xy = x * y;
    var xz = x * z;
    var yz = y * z;

    var xx = x * x;
    var yy = y * y;
    var zz = z * z;

    var sx = s * x;
    var sy = s * y;
    var sz = s * z;

    return REMakeMat4(
        +c  + xx * omc, -sz + xy * omc, +sy + xz * omc, 0,
        +sz + xy * omc, +c  + yy * omc, -sx + yz * omc, 0,
        -sy + xz * omc, +sx + yz * omc, +c  + zz * omc, 0,
        0, 0, 0, 1);
}

REMat4.prototype =
{
    copy: REMat4Copy,
    set: REMat4Set,
    set16: REMat4Set16,
    mult: REMat4Mult,
    mult16: REMat4Mult16,
    mult3: REMat4Mult3,
    mult3v: REMat4Mult3v,
    decompose: REMat4Decompose,
    sync: REMat4Sync,
    tx: REMat4Tx,
    ty: REMat4Ty,
    tz: REMat4Tz,
    shiftVec: REMat4ShiftVec,
    transpose: REMat4Transpose,
    transposed: REMat4Transposed,
    toString: REMat4ToString
};
