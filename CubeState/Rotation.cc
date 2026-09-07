#include "Rotation.h"

namespace Rotation {


// Horizontal Cube Rotation
CubeState y(const CubeState& s) {
    CubeState r;
    r.cp[3]=s.cp[0]; r.co[3]=s.co[0];
    r.cp[0]=s.cp[1]; r.co[0]=s.co[1];
    r.cp[1]=s.cp[2]; r.co[1]=s.co[2];
    r.cp[2]=s.cp[3]; r.co[2]=s.co[3];
    r.cp[7]=s.cp[4]; r.co[7]=s.co[4];
    r.cp[4]=s.cp[5]; r.co[4]=s.co[5];
    r.cp[5]=s.cp[6]; r.co[5]=s.co[6];
    r.cp[6]=s.cp[7]; r.co[6]=s.co[7];
    r.ep[0]=s.ep[1]; r.eo[0]=s.eo[1];
    r.ep[3]=s.ep[0]; r.eo[3]=s.eo[0];
    r.ep[2]=s.ep[3]; r.eo[2]=s.eo[3];
    r.ep[1]=s.ep[2]; r.eo[1]=s.eo[2];
    r.ep[4]=s.ep[5]; r.eo[4]=s.eo[5];
    r.ep[7]=s.ep[4]; r.eo[7]=s.eo[4];
    r.ep[6]=s.ep[7]; r.eo[6]=s.eo[7];
    r.ep[5]=s.ep[6]; r.eo[5]=s.eo[6];
    r.ep[11]=s.ep[8];  r.eo[11]=s.eo[8];
    r.ep[8] =s.ep[9];  r.eo[8] =s.eo[9];
    r.ep[9] =s.ep[10]; r.eo[9] =s.eo[10];
    r.ep[10]=s.ep[11]; r.eo[10]=s.eo[11];
    return r;
}

CubeState yp(const CubeState& s) { return y(y(y(s))); }
CubeState y2(const CubeState& s) { return y(y(s)); }

// Vertical Cube Rotation
CubeState x(const CubeState& s) {
    CubeState r;
    r.cp[4]=s.cp[0]; r.co[4]=(s.co[0]+2)%3;
    r.cp[7]=s.cp[4]; r.co[7]=(s.co[4]+1)%3;
    r.cp[3]=s.cp[7]; r.co[3]=(s.co[7]+2)%3;
    r.cp[0]=s.cp[3]; r.co[0]=(s.co[3]+1)%3;
    r.cp[5]=s.cp[1]; r.co[5]=(s.co[1]+1)%3;
    r.cp[6]=s.cp[5]; r.co[6]=(s.co[5]+2)%3;
    r.cp[2]=s.cp[6]; r.co[2]=(s.co[6]+1)%3;
    r.cp[1]=s.cp[2]; r.co[1]=(s.co[2]+2)%3;
    r.ep[5] =s.ep[1];  r.eo[5] =(s.eo[1]+1)%2;
    r.ep[7] =s.ep[5];  r.eo[7] =(s.eo[5]+1)%2;
    r.ep[3] =s.ep[7];  r.eo[3] =(s.eo[7]+1)%2;
    r.ep[1] =s.ep[3];  r.eo[1] =(s.eo[3]+1)%2;
    r.ep[8] =s.ep[0];  r.eo[8] =s.eo[0];
    r.ep[4] =s.ep[8];  r.eo[4] =s.eo[8];
    r.ep[11]=s.ep[4];  r.eo[11]=s.eo[4];
    r.ep[0] =s.ep[11]; r.eo[0] =s.eo[11];
    r.ep[9] =s.ep[2];  r.eo[9] =s.eo[2];
    r.ep[6] =s.ep[9];  r.eo[6] =s.eo[9];
    r.ep[10]=s.ep[6];  r.eo[10]=s.eo[6];
    r.ep[2] =s.ep[10]; r.eo[2] =s.eo[10];
    return r;
}

CubeState xp(const CubeState& s) { return x(x(x(s))); }
CubeState x2(const CubeState& s) { return x(x(s)); }

// Sideways Cube Rotation
CubeState z(const CubeState& s) {
    CubeState r;
    r.cp[1]=s.cp[0]; r.co[1]=(s.co[0]+2)%3;
    r.cp[6]=s.cp[1]; r.co[6]=(s.co[1]+1)%3;
    r.cp[7]=s.cp[6]; r.co[7]=(s.co[6]+2)%3;
    r.cp[0]=s.cp[7]; r.co[0]=(s.co[7]+1)%3;
    r.cp[2]=s.cp[3]; r.co[2]=(s.co[3]+2)%3;
    r.cp[5]=s.cp[2]; r.co[5]=(s.co[2]+1)%3;
    r.cp[4]=s.cp[5]; r.co[4]=(s.co[5]+2)%3;
    r.cp[3]=s.cp[4]; r.co[3]=(s.co[4]+1)%3;
    r.ep[9] =s.ep[1];  r.eo[9] =(s.eo[1]+1)%2;
    r.ep[5] =s.ep[9];  r.eo[5] =(s.eo[9]+1)%2;
    r.ep[8] =s.ep[5];  r.eo[8] =(s.eo[5]+1)%2;
    r.ep[1] =s.ep[8];  r.eo[1] =(s.eo[8]+1)%2;
    r.ep[2] =s.ep[0];  r.eo[2] =(s.eo[0]+1)%2;
    r.ep[6] =s.ep[2];  r.eo[6] =(s.eo[2]+1)%2;
    r.ep[4] =s.ep[6];  r.eo[4] =(s.eo[6]+1)%2;
    r.ep[0] =s.ep[4];  r.eo[0] =(s.eo[4]+1)%2;
    r.ep[10]=s.ep[3];  r.eo[10]=(s.eo[3]+1)%2;
    r.ep[7] =s.ep[10]; r.eo[7] =(s.eo[10]+1)%2;
    r.ep[11]=s.ep[7];  r.eo[11]=(s.eo[7]+1)%2;
    r.ep[3] =s.ep[11]; r.eo[3] =(s.eo[11]+1)%2;
    return r;
}

CubeState zp(const CubeState& s) { return z(z(z(s))); }
CubeState z2(const CubeState& s) { return z(z(s)); }

CubeState faceToBottom(const CubeState& s, int color) {
    switch (color) {
        case 0: return x2(s);
        case 1: return zp(s);
        case 2: return x(s);
        case 3: return s;
        case 4: return z(s);
        case 5: return xp(s);
        default: return s;
    }
}

const char* faceToBottomName(int color) {
    static const char* names[] = {"x2", "z'", "x", "", "z", "x'"};
    return names[color];
}

} // namespace Rotation