#include "Octree.hpp"
#include "Util.hpp"
#include <list>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

Octree::Node::Node() :
_p1(0, 0, 0),
_p2(0, 0, 0),
_isLeaf(true),
_live(true)
{
}

void
Octree::Node::split()
{
    _isLeaf = false;

    for(size_t i = 0; i < 8; i++)
        _children[i] = new Node;

    vec3 del = (_p2 - _p1) / 2.0;

    _children[0]->_p1 = _p1;
    _children[0]->_p2 = _p1 + del;

    _children[1]->_p1 = _children[0]->_p1;
    _children[1]->_p2 = _children[0]->_p2;
    _children[1]->_p1.x += del.x;
    _children[1]->_p2.x += del.x;

    _children[2]->_p1 = _children[1]->_p1;
    _children[2]->_p2 = _children[1]->_p2;
    _children[2]->_p1.y += del.y;
    _children[2]->_p2.y += del.y;

    _children[3]->_p1 = _children[2]->_p1;
    _children[3]->_p2 = _children[2]->_p2;
    _children[3]->_p1.x -= del.x;
    _children[3]->_p2.x -= del.x;

    for(int i = 0; i < 4; i++)
    {
        Node* n = _children[4+i];
        n->_p1 = _children[i]->_p1;
        n->_p2 = _children[i]->_p2;
        n->_p1.z += del.z;
        n->_p2.z += del.z;
    }
}

bool
Octree::Node::containsPoint(const vec3& p)
{
    if( (p.x >= _p1.x) && (p.x <= _p2.x) &&
        (p.y >= _p1.y) && (p.y <= _p2.y) &&
        (p.z >= _p1.z) && (p.z <= _p2.z) )
        return true;

    return false;
}

bool
Octree::Node::intersect(
    const vec3& p,
    const vec3& dir,
    double* dOut)
{
    if( dOut == NULL )
    {
        if( this->containsPoint(p) )
            return true;
    }

    vec3 ip;
    double d;

    // RIGHT

    d = RayToPlane(p, dir, vec3(_p2.x, 0, 0), vec3(1, 0, 0));
    if( fpclassify(d) != FP_NAN && d >= 0.0 )
    {
        ip = p + d * dir;
        if( ip.y >= _p1.y && ip.y <= _p2.y &&
            ip.z >= _p1.z && ip.z <= _p2.z )
        {
            if( dOut )
                *dOut = d;
            return true;
        }
    }

    // LEFT

    d = RayToPlane(p, dir, vec3(_p1.x, 0, 0), vec3(-1, 0, 0));
    if( fpclassify(d) != FP_NAN && d >= 0.0 )
    {
        ip = p + d * dir;
        if( ip.y >= _p1.y && ip.y <= _p2.y &&
            ip.z >= _p1.z && ip.z <= _p2.z )
        {
            if( dOut )
                *dOut = d;
            return true;
        }
    }

    // TOP

    d = RayToPlane(p, dir, vec3(0, _p2.y, 0), vec3(0, 1, 0));
    if( fpclassify(d) != FP_NAN && d >= 0.0 )
    {
        ip = p + d * dir;
        if( ip.z >= _p1.z && ip.z <= _p2.z &&
            ip.x >= _p1.x && ip.x <= _p2.x )
        {
            if( dOut )
                *dOut = d;
            return true;
        }
    }

    // BOTTOM

    d = RayToPlane(p, dir, vec3(0, _p1.y, 0), vec3(0, -1, 0));
    if( fpclassify(d) != FP_NAN && d >= 0.0 )
    {
        ip = p + d * dir;
        if( ip.z >= _p1.z && ip.z <= _p2.z &&
            ip.x >= _p1.x && ip.x <= _p2.x )
        {
            if( dOut )
                *dOut = d;
            return true;
        }
    }

    // FRONT

    d = RayToPlane(p, dir, vec3(0, 0, _p2.z), vec3(0, 0, 1));
    if( fpclassify(d) != FP_NAN && d >= 0.0 )
    {
        ip = p + d * dir;
        if( ip.x >= _p1.x && ip.x <= _p2.x &&
            ip.y >= _p1.y && ip.y <= _p2.y )
        {
            if( dOut )
                *dOut = d;
            return true;
        }
    }

    // BACK

    d = RayToPlane(p, dir, vec3(0, 0, _p1.z), vec3(0, 0, -1));
    if( fpclassify(d) != FP_NAN && d >= 0.0 )
    {
        ip = p + d * dir;
        if( ip.x >= _p1.x && ip.x <= _p2.x &&
            ip.y >= _p1.y && ip.y <= _p2.y )
        {
            if( dOut )
                *dOut = d;
            return true;
        }
    }

    return false;
}

void
Octree::Node::addPolygon(
    size_t size,
    const vec3* verts,
    const vec3& norm,
    size_t worldID)
{
    Octree::PolyData poly;
    poly.size = size;
    poly.verts = vector<vec3>(verts, verts + size);
    poly.norm = norm;
    poly.worldID = worldID;
    _polygons.push_back(poly);
}

Octree::Octree(
    int levels,
    const vec3& p1,
    const vec3& p2)
{
    _root._p1 = p1;
    _root._p2 = p2;
    _root._isLeaf = true;

    vector<Node*> toSplitA;
    vector<Node*> toSplitB;

    vector<Node*>* toSplitFront = &toSplitA;
    vector<Node*>* toSplitBack = &toSplitB;

    toSplitFront->push_back(&_root);

    levelNodes.push_back(new vector<Node*>);
    levelNodes[0]->push_back(&_root);

    for(int i = 0; i < levels; i++)
    {
        vector<Node*>* nodes = new vector<Node*>;
        levelNodes.push_back(nodes);

        for(size_t j = 0; j < toSplitFront->size(); j++)
        {
            Node* n = (*toSplitFront)[j];
            n->split();
            for(int k = 0; k < 8; k++)
            {
                Node* child = n->_children[k];
                toSplitBack->push_back(child);
                nodes->push_back(child);
            }
        }

        vector<Node*>* tmp = toSplitFront;
        toSplitFront = toSplitBack;
        toSplitBack = tmp;

        toSplitBack->clear();
    }

    vec3 del = p2 - p1;
    double min = del.x;

    if( del.y < min )
        min = del.y;

    if( del.z < min )
        min = del.z;
}

bool
Octree::addPolygon(
    size_t size,
    const vec3* verts,
    const vec3& norm,
    size_t worldID)
{
    vector<Node*>& nodes = *(levelNodes.back());

    bool needsFitTest = true;
    int added = 0;

    for(size_t i = 0; i < nodes.size(); i++)
    {
        Node& node = *(nodes[i]);

        if( needsFitTest )
        {
            bool fit = true;
            for(size_t j = 0; j < size; j++)
            {
                if( ! node.containsPoint(verts[j]) )
                {
                    fit = false;
                    break;
                }
            }

            if( fit )
            {
                node.addPolygon(size, verts, norm, worldID);
                return true;
            }
        }

        for(size_t j = 0; j < size; j++)
        {
            const vec3& v1 = verts[j];
            const vec3& v2 = verts[(j+1) % size];

            vec3 dir = v2 - v1;
            double d;

            if( node.intersect(v1, dir, &d) )
            {
                vec3 ip = v1 + d * dir;

                vec3 f1 = ip - v1;
                vec3 f2 = ip - v2;

                if( f1 * f2 < 0.0 )
                {
                    added++;
                    needsFitTest = false;
                    node.addPolygon(size, verts, norm, worldID);
                    break;
                }
            }
        }
    }

    if( ! added )
        ErrorQuit(FL, "polygon not added");

    if( added )
        return true;

    return false;
}

bool
Octree::Node::recurse(const vec3& p, const vec3& dir, size_t worldID, double* dOut)
{
    if( ! _live )
        return false;

    if( ! this->intersect(p, dir) )
        return false;

    for(size_t i = 0; i < _polygons.size(); i++)
    {
        PolyData& poly = _polygons[i];

        if( worldID == poly.worldID )
            continue;

        double d = RayToPlane(
            p, dir, poly.verts[0], poly.norm);

        if( fpclassify(d) == FP_NAN || d < 0.0 )
            continue;

        vec3 ip = p + d * dir;

        bool hit = PointInPoly(
            ip, poly.size, &poly.verts[0], poly.norm);

        if( hit )
        {
            if( dOut )
                *dOut = d;
            return true;
        }
    }

    if( _isLeaf )
        return false;

    for(size_t i = 0; i < 8; i++)
    {
        if( _children[i]->recurse(p, dir, worldID, dOut) )
            return true;
    }

    return false;
}

void
Octree::Node::status(int level)
{
    if( (int)_polygons.size() )
    {
        for(int x = 0; x < level + 1; x++)
            printf(">");

        printf(" %d %d", level + 1, (int)_polygons.size());
        printf(" (%f, %f)", _p1.x, _p2.x);
        printf(" (%f, %f)", _p1.y, _p2.y);
        printf(" (%f, %f)\n", _p1.z, _p2.z);
    }

    if( _isLeaf )
        return;

    for(size_t i = 0; i < 8; i++)
        _children[i]->status(level + 1);
}

void
Octree::Node::finalize()
{
    if( _isLeaf )
    {
        _live = _polygons.size() ? true : false;
    }
    else
    {
        for(size_t i = 0; i < 8; i++)
            _children[i]->finalize();

        for(size_t i = 0; i < 8; i++)
        {
            if( _children[i]->_live )
            {
                _live = true;
                return;
            }
        }
    }
}

void
Octree::finalize()
{
    _root.finalize();
}

bool
Octree::intersect(const vec3& p, const vec3& dir, size_t worldID, double* dOut)
{
    return _root.recurse(p, dir, worldID, dOut);
}
