#include "Vec3.hpp"
#include <vector>

class Octree
{
public:

    struct PolyData
    {
        size_t size;
        std::vector<vec3> verts;
        vec3 norm;
        size_t worldID;
    };

    class Node
    {
    public:
        vec3 _p1;
        vec3 _p2;
        bool _isLeaf;
        Node* _children[8];
        std::vector<PolyData> _polygons;
        bool _live;

        Node();
        void split();
        bool intersect(const vec3& p, const vec3& dir, double* dOut = NULL);
        bool recurse(const vec3& p, const vec3& dir, size_t worldID, double *dOut);
        bool containsPoint(const vec3& p);
        void status(int level);
        void finalize();
        void addPolygon(
            size_t size,
            const vec3* verts,
            const vec3& norm,
            size_t worldID);
    };

private:
    std::vector<std::vector<Node*>*> levelNodes;
    Node _root;

public:
    Octree(
        int levels,
        const vec3& p1,
        const vec3& p2);

    void finalize();

    bool addPolygon(
        size_t size,
        const vec3* verts,
        const vec3& norm,
        size_t worldID);

    bool intersect(
        const vec3& p,
        const vec3& dir,
        size_t worldID,
        double* dOut = NULL);
};
