#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <float.h>
#include <sys/time.h>

#include <string>
#include <map>
#include <vector>
#include <list>

#include "Vec3.hpp"
#include "Mat4.hpp"
#include "Util.hpp"
#include "ModelReader.hpp"
#include "Octree.hpp"

const char* progName;
const char* inputFileName;

ModelReader reader;

//const double SPA = 2500;
//const size_t hemiCount = 64;

double SPA = 3000 * 0.5;
const size_t hemiCount = 64;
const int subdivs = 4;

vec3 hemis[hemiCount];

using namespace std;

struct PolyData
{
    size_t idx;
    int size;
    vec3 norm;
};

vector<vec3> verts;
vector<PolyData> polygons;

Octree* octree;

#define SC_SIZE 512

class StabilizationController
{
public:
    double _tolerance;
    size_t _count;
    
    double _samples[512];

    size_t _idx;
    bool _filled;

    StabilizationController(size_t pCount, double pTolerance);
    void addSample(double value);
    bool isStable();
};

StabilizationController::StabilizationController(
    size_t pCount,
    double pTolerance)
{
    if( pCount > SC_SIZE )
        ErrorQuit(FL, "pCount > SC_SIZE");
    
    _count = pCount;
    _tolerance = pTolerance;
    _idx = 0;
}

void
StabilizationController::addSample(double value)
{
    _samples[_idx] = value;

    _idx++;

    if( _idx == _count )
    {
        _filled = true;
        _idx = 0;
    }
}

bool
StabilizationController::isStable()
{
    if( ! _filled )
        return false;

    double min = +DBL_MAX; 
    double max = -DBL_MAX;

    for(size_t i = 0; i < _count; i++)
    {
        double s = _samples[i];

        if( s < min )
            min = s;

        if( s > max )
            max = s;
    }

    if( max - min < _tolerance )
        return true;

    return false;
}

void
WriteFile(int fd, const void* buff, size_t count)
{
    ssize_t ret = write(fd, buff, count);
    if( ret < 0 || (size_t)ret != count )
    {
        printf("ERROR: write() failed\n");
        exit(1);
    }
}

double
GetTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
}

double
ProcessPoint(
    size_t wip,
    const vec3& p)
{
    PolyData& poly = polygons[wip];

    int nOccluded = 0;

    for(size_t h = 0; h < hemiCount; h++)
    {
        vec3 dir;

        while(true)
        {
            dir = vec3(
                -1.0 + 2.0 * (rand() / (double)RAND_MAX),
                -1.0 + 2.0 * (rand() / (double)RAND_MAX),
                -1.0 + 2.0 * (rand() / (double)RAND_MAX));

            if( dir.length2() > 1.0 )
                continue;

            if( dir * poly.norm <= 0.0 )
                continue;

            break;
        }

        if( octree->intersect(p, dir, wip) )
            nOccluded++;
    }

    return 1.0 - (double)nOccluded / (double)hemiCount;
}

void
ReadFile(void** data, size_t* size)
{
    int fd = open(inputFileName, O_RDONLY);
    if( fd == -1 )
        ErrorQuit(FL, "open() failed");

    struct stat st;
    if( fstat(fd, &st) == -1 )
        ErrorQuit(FL, "fstat() failed");

    *data = (char*)malloc(st.st_size);
    ssize_t readRet = read(fd, *data, st.st_size);
    if( readRet == -1 )
        ErrorQuit(FL, "read() failed\n");

    *size = st.st_size;
    close(fd);
}

void
usage()
{
    printf("Usage: %s <input.re> <SPA>\n", progName);
    exit(1);
}

int
main(int argc, const char** argv)
{
    progName = argv[0];

    printf("%d\n", argc);
    if( argc < 3 )
        usage();

    inputFileName = argv[1];
    SPA = atof(argv[2]);

    if( SPA == 0.0 )
        usage();

    void* fileData;
    size_t fileDataSize;

    printf("HERE!*\n");
    ReadFile(&fileData, &fileDataSize);
    reader.load(fileData, fileDataSize);
    printf("HERE!\n");

    vec3 p1(+DBL_MAX, +DBL_MAX, +DBL_MAX);
    vec3 p2(-DBL_MAX, -DBL_MAX, -DBL_MAX);

    printf("HERE!<\n");

    for(size_t i = 0; i < reader.objectCount(); i++)
    {
        printf("%d\n", (int)i);
        ObjectData& obj = reader.objectByIndex(i);

        MeshData& mesh = reader.meshByName(obj.meshName);
        int re_Position = mesh.attribOffset("re_Position");
        int re_Normal = mesh.attribOffset("re_Normal");

        printf("%d\n", (int)i);
        for(size_t j = 0; j < mesh.polygonCount(); j++)
        {
            size_t ps = mesh.polygonSize(j);
            for(size_t k = 0; k < ps; k++)
            {
                vec3 pos = mesh.vertVec3(j, k, re_Position);
                pos = obj.basis * pos;

                if( pos.x > p2.x ) p2.x = pos.x;
                if( pos.x < p1.x ) p1.x = pos.x;
                if( pos.y > p2.y ) p2.y = pos.y;
                if( pos.y < p1.y ) p1.y = pos.y;
                if( pos.z > p2.z ) p2.z = pos.z;
                if( pos.z < p1.z ) p1.z = pos.z;
            }
        }
    }

    printf("HERE!\n");

    // TOOD: This is wrong if the origin is not in the octree space
    //p1 = 1.05 * p1;
    //p2 = 1.05 * p2;

    printf("%f, %f, %f\n", p1.x, p1.y, p1.z);
    printf("%f, %f, %f\n", p2.x, p2.y, p2.z);

    Octree ot(subdivs, p1, p2);
    octree = &ot;

    size_t worldIndex = 0;
    for(size_t i = 0; i < reader.objectCount(); i++)
    {
        ObjectData& obj = reader.objectByIndex(i);
        MeshData& mesh = reader.meshByName(obj.meshName);
        int re_Position = mesh.attribOffset("re_Position");
        int re_Normal = mesh.attribOffset("re_Normal");

        int added = 0;
        for(size_t j = 0; j < mesh.polygonCount(); j++)
        {
            size_t ps = mesh.polygonSize(j);

            PolyData pd;

            pd.idx = verts.size();
            pd.size = ps;
            pd.norm = mesh.vertVec3(j, 0, re_Normal);

            polygons.push_back(pd);

            for(size_t k = 0; k < ps; k++)
            {
                vec3 pos = mesh.vertVec3(j, k, re_Position);
                pos = obj.basis * pos;
                verts.push_back(pos);
            }

            if( octree->addPolygon(
                pd.size, &verts[pd.idx], pd.norm, worldIndex++) )
                added++;

            printf(
                "%.2f [%d] [%d]\n",
                100.0 * j / (double)mesh.polygonCount(),
                (int)mesh.polygonCount(), added);
        }
    }

    octree->finalize();

    worldIndex = 0;
    for(size_t i = 0; i < reader.objectCount(); i++)
    {
        ObjectData& obj = reader.objectByIndex(i);
        MeshData& mesh = reader.meshByName(obj.meshName);

        if( obj.meshName != "Grid" )
            continue;

        printf("Mesh [%s]\n", obj.meshName.c_str());

        size_t wi = worldIndex;
        worldIndex += mesh.polygonCount();

        vector<uint8_t> visOut;

        /*
        size_t nTakeAO = 0;
        for(size_t j = 0; j < mesh.polygonCount(); j++)
            if( mesh.polygonTakesAO(j) )
                nTakeAO++;
         */

        for(size_t j = 0; j < mesh.polygonCount(); j++)
        {
            /*
            if( ! mesh.polygonTakesAO(j) )
            {
                for(size_t k = 0; k < mesh.polygonSize(j); k++)
                    visOut[i]->push_back(1.0);
                printf("%.2f\n", 100.0 * (double)j / ((double)mesh.polygonCount() - 1));
                continue;
            }
            */

            size_t wip = wi + j;
            PolyData& poly = polygons[wip];

            vec3 p1 = verts[poly.idx + 0];
            vec3 p2 = verts[poly.idx + 1];

            vec3 zAxis = poly.norm;
            vec3 xAxis = (p2 - p1).normal();
            vec3 yAxis = zAxis.cross(xAxis);

            double area = 0.0;

            for(size_t k = 0; k < mesh.polygonSize(j) - 2; k++)
            {
                size_t i1 = poly.idx;
                size_t i2 = poly.idx + k + 1;
                size_t i3 = poly.idx + k + 2;

                vec3 t1 = verts[i1];
                vec3 t2 = verts[i2];
                vec3 t3 = verts[i3];

                double tArea = poly.norm * (t2 - t1).cross(t3 - t1);

                if( tArea < 0.0 )
                    tArea = 0.0;

                area += tArea;
            }

            if( area < 0.0 )
            {
                printf("ERROR: Polygon area less than 0.0\n");
                exit(1);
            }

            double bounds[6] = {
                +DBL_MAX, -DBL_MAX,
                +DBL_MAX, -DBL_MAX,
                +DBL_MAX, -DBL_MAX
            };

            for(size_t k = 0; k < mesh.polygonSize(j); k++)
            {
                vec3 pt = verts[poly.idx + k];

                if( pt.x < bounds[0] ) bounds[0] = pt.x;
                if( pt.x > bounds[1] ) bounds[1] = pt.x;
                if( pt.y < bounds[2] ) bounds[2] = pt.y;
                if( pt.y > bounds[3] ) bounds[3] = pt.y;
                if( pt.z < bounds[4] ) bounds[4] = pt.z;
                if( pt.z > bounds[5] ) bounds[5] = pt.z;
            }

            vec3 diag(
                bounds[1] - bounds[0],
                bounds[3] - bounds[2],
                bounds[5] - bounds[4]);

            double maxDist = diag.x;
            if( diag.y > maxDist ) maxDist = diag.y;
            if( diag.z > maxDist ) maxDist = diag.z;

            int samples = round(SPA * area);

            if( samples == 0 )
                samples = 1;

            double totalVis = 0.0;

            //printf("samples: %d [%f] [%f]\n", samples, SPA, area);

            int samplesProcessed = 0;
            while(samplesProcessed < samples)
            {
                double r1 = maxDist * (rand() / (double)RAND_MAX);
                double r2 = maxDist * (rand() / (double)RAND_MAX);

                vec3 pt = p1 + r1 * xAxis + r2 * yAxis;

                if( ! PointInPoly(
                        pt,
                        poly.size,
                        &verts[poly.idx],
                        poly.norm) )
                {
                    continue;
                }

                totalVis += ProcessPoint(wip, pt);
                samplesProcessed++;
            }

            double avgVis = totalVis / (double)samples;

            for(size_t k = 0; k < mesh.polygonSize(j); k++)
            {
                uint8_t value = 255 * avgVis;
                visOut.push_back(value);
            }

            printf("%.2f\n", 100.0 * (double)j / ((double)mesh.polygonCount() - 1));
        }

        obj.gpuBufferSize = visOut.size();
        obj.gpuBuffer = malloc(obj.gpuBufferSize);
        memcpy(obj.gpuBuffer, &visOut[0], obj.gpuBufferSize);

        AttribDescriptor as;
        as.name = "re_AmbientOcclusion";
        as.type = 1;
        as.dimen = 1;
        as.normalized = 1;
        as.offset = 0;

        obj.attribs.push_back(as);
        obj.stride += 1;
    }

    printf("writing output\n");

    vector<char> output;
    reader.compose(output);

    int fd = open(
        inputFileName,
        O_WRONLY | O_TRUNC | O_CREAT,
        0644);

    if( fd == -1 )
        ErrorQuit(FL, "ERROR: open() failed");

    WriteFile(fd, &output[0], output.size());
    close(fd);

    return 0;
}
