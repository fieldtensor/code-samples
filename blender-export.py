import bpy
import sys
import struct
import os

class EdgeInfo:
    def __init__(self):
        self.verts = []
        self.normals = []

blenderFileName = None
exportFileName = None

ErrorBuff = ""
ErrorFound = False
ErrorList = []

def BeginError():
    global ErrorBuff
    global ErrorFound
    ErrorFound = True
    ErrorBuff = ""

def EndError():
    global ErrorBuff
    global ErrorList
    ErrorList.append(ErrorBuff)
    ErrorBuff = ""

def LogError(msg):
    global ErrorBuff
    ErrorBuff += msg + "\n"

def PrintInBox(title):
    tlen = len(title)
    print("+", end="")
    print("-" * (tlen + 2), end="")
    print("+", end="\n")
    print("| ", end="")
    print(title, end="");

    print(" |", end="\n")
    print("+", end="")
    print("-" * (tlen + 2), end="")
    print("+", end="\n")

def WriteBytes(f, data):
    count = len(data)
    f.write(struct.pack("<I", count))
    f.write(data)

    mod = count % 4
    if mod:
        pad = 4 - count % 4
        while(pad):
            f.write(struct.pack("<B", 0))
            pad -= 1

def WriteString(f, s):
    WriteBytes(f, bytes(s, 'utf-8'))

def FormatVertex(
    vx, vy, vz,
    nx, ny, nz,
    diffRed, diffGreen, diffBlue,
    vnx, vny, vnz):

    output = bytes()
    output += struct.pack("<fff", vx, vy, vz)
    output += struct.pack("<fff", nx, ny, nz)
    output += struct.pack("<fff", vnx, vny, vnz)
    output += struct.pack(
        "<BBBB",
        int(255 * diffRed),
        int(255 * diffGreen),
        int(255 * diffBlue),
        255);

    return output

def AnyObj(mesh):
    for obj in bpy.data.objects:
        if obj.data == mesh:
            return obj
    return None

meshCache = {}
def GroupName(mesh, idx):
    obj = AnyObj(mesh)

    if mesh.name in meshCache:
        idxCache = meshCache[mesh.name]
        if idx in idxCache:
            return idxCache[idx]

    for g in obj.vertex_groups:
        if g.index == idx:

            idxCache = None
            if mesh.name not in meshCache:
                idxCache = {}
                meshCache[mesh.name] = idxCache
            else:
                idxCache = meshCache[mesh.name]

            idxCache[idx] = g.name
            return g.name

    return None

def GroupWeight(mesh, v, grp, default):
    for g in v.groups:
        if grp == GroupName(mesh, g.group):
            return g.weight
    return default

def ObjectForMesh(meshName):
    for obj in bpy.data.objects:
        if obj.type != "EMPTY" and obj.data.name == meshName:
            return obj
    return None

def run():
    f = open(exportFileName, "wb");

    print("Writing meshes...")

    meshes = bpy.data.meshes
    for mesh in meshes:

        objName = ObjectForMesh(mesh.name).name
        if objName == "CameraFocus" or objName == "CameraPosition":
            continue

        if mesh.name == "CausticMask":
            continue

        #PrintInBox(mesh.name)
        #print("")
        print("  " + mesh.name)

        uvLayer = None
        if len(mesh.uv_layers):
            uvLayer = mesh.uv_layers[0].data
        verts = mesh.vertices
        loops = mesh.loops

        f.write(struct.pack("<I", 0))
        WriteString(f, mesh.name)

        vidx = 0
        vertBuff = bytes()
        elementBuff = bytes()
        polyBuff = bytes()
        edgeBuff = bytes()

        edges = {}

        for poly in mesh.polygons:

            mat = mesh.materials[poly.material_index]

            if mat is None:
                print("ERROR: Found empty material on mesh " + mesh.name);
                os.remove(exportFileName)
                sys.exit(1)
                

            start = poly.loop_start
            end = poly.loop_start + poly.loop_total
            vcount = end - start

            if vcount < 3:
                print("ERROR: Polygon found with fewer than 3 vertices");
                os.remove(exportFileName)
                sys.exit(1)

            isDeepOcean = True
            for i in range(start, end):
                vi = loops[i].vertex_index
                vert = verts[vi]
                dg = GroupWeight(mesh, vert, "DeepGround", 0.0)
                if dg == 0.0:
                    isDeepOcean = False
                    break

            takesAO = True
            for i in range(start, end):
                vi = loops[i].vertex_index
                vert = verts[vi]
                val = GroupWeight(mesh, vert, "takesAO", 1.0)
                if val == 0.0:
                    takesAO = False
                    break

            if isDeepOcean:
                continue

            for i in range(start, end):
                vi = loops[i].vertex_index
                vert = verts[vi]

                u = 0.0
                v = 0.0

                if uvLayer is not None:
                    uv = uvLayer[i].uv
                    u = uv.x
                    v = uv.y

                vertBuff += FormatVertex(
                    vert.co.x,
                    vert.co.y,
                    vert.co.z,
                    poly.normal.x,
                    poly.normal.y,
                    poly.normal.z,
                    mat.diffuse_color.r,
                    mat.diffuse_color.g,
                    mat.diffuse_color.b,
                    vert.normal.x,
                    vert.normal.y,
                    vert.normal.z);

            N = end - start
            for i in range(start, end):
                j = i - start

                vi = loops[start + j].vertex_index
                vin = loops[start + (j+1)%N].vertex_index
                a = verts[vi]
                b = verts[vin]

                edgeID = None
                if vi < vin:
                    edgeID = str(vi) + "," + str(vin)
                else:
                    edgeID = str(vin) + "," + str(vi)

                if not edgeID in edges:
                    newInfo = EdgeInfo()
                    newInfo.verts.append(a.co)
                    newInfo.verts.append(b.co)
                    edges[edgeID] = newInfo

                info = edges[edgeID]
                info.normals.append(poly.normal)

                if poly.normal.length == 0.0:
                    obj = ObjectForMesh(mesh.name)
                    p1 = obj.matrix_world * a.co
                    p2 = obj.matrix_world * b.co
                    
                    BeginError()
                    LogError("x: %f\ny: %f\nz: %f" %
                          ((p1.x+p2.x)/2, (p1.y+p2.y)/2, (p1.z+p2.z)/2))
                    LogError("ERROR: Zero length normal found")
                    EndError()

            for j in range(0, vcount - 2):
                elementBuff += struct.pack(
                    "<HHH", vidx, vidx + j + 1, vidx + j + 2)

            polyBuff += struct.pack(
                "<HH", vcount, vidx)

            vidx += vcount 
            if( vidx % 500 == 0 ):
                print("vert " + str(vidx))

        for edgeID in edges:
            edge = edges[edgeID]

            faceCount = len(edge.normals)
            if faceCount != 2:
                obj = ObjectForMesh(mesh.name)
                p1 = obj.matrix_world * edge.verts[0]
                p2 = obj.matrix_world * edge.verts[1]
                
                BeginError()
                LogError("ERROR: Edge found with %d faces" % faceCount)
                LogError(edgeID)
                LogError("x: %f\ny: %f\nz: %f" %
                      ((p1.x+p2.x)/2, (p1.y+p2.y)/2, (p1.z+p2.z)/2))
                EndError()
            else:
                edgeBuff += struct.pack("<ffffffffffff",
                    edge.verts[0].x, edge.verts[0].y, edge.verts[0].z,
                    edge.verts[1].x, edge.verts[1].y, edge.verts[1].z,
                    edge.normals[0].x, edge.normals[0].y, edge.normals[0].z,
                    edge.normals[1].x, edge.normals[1].y, edge.normals[1].z)

        f.write(struct.pack("<I", 4))

        WriteString(f, "re_Position");
        f.write(struct.pack("<III", 0, 3, 0))

        WriteString(f, "re_Normal");
        f.write(struct.pack("<III", 0, 3, 0))

        WriteString(f, "re_VertNormal");
        f.write(struct.pack("<III", 0, 3, 0))

        WriteString(f, "re_DiffuseColor");
        f.write(struct.pack("<III", 1, 4, 1))

        WriteBytes(f, vertBuff)
        WriteBytes(f, elementBuff)
        WriteBytes(f, polyBuff)
        print("Edge Indexes [%s] %f\n" % (mesh.name, 4 * len(edges) / 2**16))
        WriteBytes(f, edgeBuff)

        print(len(edges))
        print(len(edges))

    print("")
    print("Writing objects...")

    objects = bpy.data.objects
    for obj in objects:

        if obj.type != "MESH" and obj.type != "EMPTY":
            continue

        #PrintInBox(obj.name)
        #print("")
        print("  " + obj.name)

        mesh = obj.data
        m = obj.matrix_basis

        if obj.name == "CameraFocus":
            f.write(struct.pack("<I", 2))
            f.write(struct.pack("<fff", m[0][3], m[1][3], m[2][3]))
            continue

        if obj.name == "CameraPosition":
            f.write(struct.pack("<I", 3))
            f.write(struct.pack("<fff", m[0][3], m[1][3], m[2][3]))
            continue

        if obj.name == "CausticMask":
            continue

        if obj.type == "EMPTY":
            print("ERROR: Unknown empty named " + obj.name)
            os.remove(exportFileName)
            sys.exit(1)

        if not len(mesh.polygons):
            print("ERROR: Empty mesh named " +
                  mesh.name + " on object " + obj.name)
            os.remove(exportFileName)
            sys.exit(1)

        f.write(struct.pack("<I", 1))

        WriteString(f, obj.name)
        WriteString(f, mesh.name)

        f.write(struct.pack("<ffff", m[0][0], m[0][1], m[0][2], m[0][3]))
        f.write(struct.pack("<ffff", m[1][0], m[1][1], m[1][2], m[1][3]))
        f.write(struct.pack("<ffff", m[2][0], m[2][1], m[2][2], m[2][3]))
        f.write(struct.pack("<ffff", m[3][0], m[3][1], m[3][2], m[3][3]))

        f.write(struct.pack("<I", 0)) # Attrib count
        f.write(struct.pack("<I", 0)) # GPUBUffer size

        #print("mat %+0.2f, %+0.2f, %+0.2f, %+0.2f" % (
        #      m[0][0], m[0][1], m[0][2], m[0][3]))
        #print(">>> %+0.2f, %+0.2f, %+0.2f, %+0.2f" % (
        #      m[1][0], m[1][1], m[1][2], m[1][3]))
        #print(">>> %+0.2f, %+0.2f, %+0.2f, %+0.2f" % (
        #      m[2][0], m[2][1], m[2][2], m[2][3]))
        #print(">>> %+0.2f, %+0.2f, %+0.2f, %+0.2f" % (
        #      m[3][0], m[3][1], m[3][2], m[3][3]))
        #print("")

    f.flush()

firstArg = 0
while firstArg < len(sys.argv):
    firstArg += 1
    if sys.argv[firstArg-1] == "--":
        break

i = 0
while True:
    j = firstArg + i
    if j >= len(sys.argv):
        break
    i += 1

    if i == 1:
        blenderFileName = sys.argv[j]
    elif i == 2:
        exportFileName = sys.argv[j]

if bpy.ops.object.mode_set.poll():
    bpy.ops.object.mode_set(mode='OBJECT')
else:
    print("ERROR: poll() failed")
    os.remove(exportFileName)
    sys.exit(1)

if exportFileName is None:
    print("");
    print("+------------------------------------------+");
    print("| Usage: make-model <file.blend> <file.re> |");
    print("+------------------------------------------+");
    print("");
else:
    run()

    if ErrorFound:
        print("+---------------------------+")
        print("| Geometry Errors Detected! |")
        print("+---------------------------+")
        print("\n")
        ErrorList.sort()
        for e in ErrorList:
            print(e)
        os.remove(exportFileName)
