#include "Rotation.h"
#include "MoveTable.h"

// Rotation Tables
namespace {

// Corner Positions: URF=0 UFL=1 ULB=2 UBR=3 DFR=4 DLF=5 DBL=6 DRB=7
// Edge Positions:   UR=0 UF=1 UL=2 UB=3 DR=4 DF=5 DL=6 DB=7 FR=8 FL=9 BL=10 BR=11

const MoveTable BASE_ROTATIONS[3] = {
    // x — F to U, Corners as R and L', Edges as R, M' and L'
    {
        {4,5,1,0, 7,6,2,3},
        {2,1,2,1, 1,2,1,2},
        {8,5,9,1, 11,7,10,3, 4,6,2,0},
        {0,1,0,1, 0,1,0,1, 0,0,0,0}
    },
    // y — R to F, Corners as U and D', Edges as U, E' and D'
    {
        {3,0,1,2, 7,4,5,6},
        {0,0,0,0, 0,0,0,0},
        {3,0,1,2, 7,4,5,6, 11,8,9,10},
        {0,0,0,0, 0,0,0,0, 1,1,1,1}
    },
    // z — U to R, Corners as F and B', Edges as F, S and B'
    {
        {1,5,6,2, 0,4,7,3},
        {1,2,1,2, 2,1,2,1},
        {2,9,6,10, 0,8,4,11, 1,5,7,3},
        {1,1,1,1, 1,1,1,1, 1,1,1,1}
    },
};

// Which old view face each new view face shows, U R F D L B
const uint8_t BASE_FACES[3][6] = {
    {2, 1, 3, 5, 4, 0},   // x: U←F, D←B, B←U, F←D
    {0, 5, 1, 3, 2, 4},   // y: R←B, F←R, L←F, B←L
    {4, 0, 2, 1, 3, 5},   // z: U←L, R←U, D←R, L←D
};

// Physical Motion of the Whole Cube, as a State Composed After Another
MoveTable composeMotion(const MoveTable& a, const MoveTable& b) {
    MoveTable r;
    for (int i = 0; i < 8; i++) {
        r.cp[i] = a.cp[b.cp[i]];
        r.co[i] = (a.co[b.cp[i]] + b.co[i]) % 3;
    }
    for (int i = 0; i < 12; i++) {
        r.ep[i] = a.ep[b.ep[i]];
        r.eo[i] = (a.eo[b.ep[i]] + b.eo[i]) % 2;
    }
    return r;
}

MoveTable invert(const MoveTable& t) {
    MoveTable r;
    for (int i = 0; i < 8; i++) {
        r.cp[t.cp[i]] = i;
        r.co[t.cp[i]] = (3 - t.co[i]) % 3;
    }
    for (int i = 0; i < 12; i++) {
        r.ep[t.ep[i]] = i;
        r.eo[t.ep[i]] = t.eo[i];
    }
    return r;
}

// The Rotation Group, Built Once by Closing {x, y, z} Over Composition
struct Group {
    int       count = 0;
    uint8_t   faces[24][6];       // View Face → Physical Face
    MoveTable motion[24];         // Physical Motion From the Identity
    MoveTable unmotion[24];       // Its Inverse
    uint8_t   then[24][3];        // Element After a Base Rotation
    uint8_t   compose[24][24];
    uint8_t   inverse[24];
    uint8_t   byRot[9];           // CubeRot → Element
    std::string name[24];         // Shortest Rotation Sequence From the Identity

    Group() {
        for (int f = 0; f < 6; f++) faces[0][f] = f;
        for (int i = 0; i < 8;  i++) { motion[0].cp[i] = i; motion[0].co[i] = 0; }
        for (int i = 0; i < 12; i++) { motion[0].ep[i] = i; motion[0].eo[i] = 0; }
        count = 1;

        // Breadth-first closure, every element reached within three base rotations
        for (int i = 0; i < count; i++) {
            for (int b = 0; b < 3; b++) {
                uint8_t f[6];
                for (int v = 0; v < 6; v++) f[v] = faces[i][BASE_FACES[b][v]];
                int j = find(f);
                if (j < 0) {
                    j = count++;
                    for (int v = 0; v < 6; v++) faces[j][v] = f[v];
                    motion[j] = composeMotion(motion[i], BASE_ROTATIONS[b]);
                }
                then[i][b] = j;
            }
        }

        for (int i = 0; i < count; i++) unmotion[i] = invert(motion[i]);

        for (int a = 0; a < count; a++) {
            for (int b = 0; b < count; b++) {
                uint8_t f[6];
                for (int v = 0; v < 6; v++) f[v] = faces[a][faces[b][v]];
                compose[a][b] = find(f);
            }
            for (int b = 0; b < count; b++) {
                if (compose[a][b] == 0) inverse[a] = b;
            }
        }

        // CubeRot Order Is x xp x2 y yp y2 z zp z2
        for (int axis = 0; axis < 3; axis++) {
            int once   = then[0][axis];
            int twice  = then[once][axis];
            int thrice = then[twice][axis];
            byRot[axis * 3 + 0] = once;
            byRot[axis * 3 + 1] = thrice;
            byRot[axis * 3 + 2] = twice;
        }

        // Breadth-first again over all nine rotations, so names are as short as
        // possible and single rotations win over pairs
        bool named[24] = {true};
        int  order[24] = {0};
        int  head = 0, tail = 1;
        while (head < tail) {
            int i = order[head++];
            for (int r = 0; r < 9; r++) {
                int j = compose[i][byRot[r]];
                if (named[j]) continue;
                named[j] = true;
                name[j]  = name[i].empty() ? rotationName(static_cast<CubeRot>(r))
                                           : name[i] + " " + rotationName(static_cast<CubeRot>(r));
                order[tail++] = j;
            }
        }
    }

    int find(const uint8_t f[6]) const {
        for (int i = 0; i < count; i++) {
            bool same = true;
            for (int v = 0; v < 6; v++) if (faces[i][v] != f[v]) same = false;
            if (same) return i;
        }
        return -1;
    }
};

const Group& group() {
    static const Group g;
    return g;
}

} // namespace

// Orientation

Orientation Orientation::then(CubeRot r) const {
    return then(fromRotation(r));
}

Orientation Orientation::then(const Orientation& o) const {
    Orientation result;
    result.id = group().compose[id][o.id];
    return result;
}

Orientation Orientation::inverse() const {
    Orientation result;
    result.id = group().inverse[id];
    return result;
}

int Orientation::faceAt(int viewFace) const {
    return group().faces[id][viewFace];
}

Orientation Orientation::fromRotation(CubeRot r) {
    Orientation result;
    result.id = group().byRot[static_cast<int>(r)];
    return result;
}

Move translateMove(Move m, const Orientation& o) {
    int idx  = static_cast<int>(m);
    int face = o.faceAt(idx / 3);
    return static_cast<Move>(face * 3 + idx % 3);
}

// Frame change is conjugation by the physical motion: undo it, keep the state, redo it
// Pieces land in their new slots and are renamed by where the motion put their homes
CubeState CubeState::rotate(const Orientation& o) const {
    const MoveTable& m  = group().motion[o.id];
    const MoveTable& mi = group().unmotion[o.id];
    CubeState next;
    for (int i = 0; i < 8; i++) {
        int mid    = m.cp[i];
        int src    = cp[mid];
        next.cp[i] = mi.cp[src];
        next.co[i] = (mi.co[src] + co[mid] + m.co[i]) % 3;
    }
    for (int i = 0; i < 12; i++) {
        int mid    = m.ep[i];
        int src    = ep[mid];
        next.ep[i] = mi.ep[src];
        next.eo[i] = (mi.eo[src] + eo[mid] + m.eo[i]) % 2;
    }
    return next;
}

CubeState CubeState::rotate(CubeRot r) const {
    return rotate(Orientation::fromRotation(r));
}

// Which Rotation Puts Each Colour on D, in Colour Order
static const CubeRot TO_BOTTOM[6] = {
    CubeRot::x2,   // white  (U)
    CubeRot::z,    // red    (R)
    CubeRot::xp,   // green  (F)
    CubeRot::x,    // yellow (D), Unused
    CubeRot::zp,   // orange (L)
    CubeRot::x,    // blue   (B)
};

Orientation orientationWithBottom(int color) {
    if (color == 3) return Orientation{};
    return Orientation::fromRotation(TO_BOTTOM[color]);
}

// Names

const char* rotationName(CubeRot r) {
    static const char* names[] = {"x", "x'", "x2", "y", "y'", "y2", "z", "z'", "z2"};
    return names[static_cast<int>(r)];
}

const char* colorName(int color) {
    static const char* names[] = {"white", "red", "green", "yellow", "orange", "blue"};
    return names[color];
}

std::string rotationsTo(const Orientation& o) {
    return group().name[o.id];
}

std::string orientationName(const Orientation& o) {
    static const char* faces = "URFDLB";
    std::string result;
    for (int i = 0; i < 6; i++) {
        if (i > 0) result += "  ";
        result += faces[i];
        result += ' ';
        result += colorName(o.faceAt(i));
    }
    return result;
}
