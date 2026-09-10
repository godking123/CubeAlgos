#include "SolverState.h"

namespace {

// faces: U=0 R=1 F=2 D=3 L=4 B=5

static const uint8_t X_FACES[6] = {2, 1, 3, 5, 4, 0};
//                                  U  R  F  D  L  B
// x: U←F, R←R, F←D, D←B, L←L, B←U

static const uint8_t Y_FACES[6] = {0, 5, 1, 3, 2, 4};
//                                  U  R  F  D  L  B
// y: U←U, R←B, F←R, D←D, L←F, B←L

static const uint8_t Z_FACES[6] = {4, 0, 2, 1, 3, 5};
//                                  U  R  F  D  L  B
// z: U←L, R←U, F←F, D←R, L←D, B←B

// Use Rotation Arrays to Remap View
void applyFaceMap(uint8_t faces[6], const uint8_t map[6]) {
    uint8_t tmp[6];
    for (int i = 0; i < 6; i++)
        tmp[i] = faces[map[i]];
    for (int i = 0; i < 6; i++)
        faces[i] = tmp[i];
}

} // namespace

// Rotate View Dependent on Rotation Type
Orientation rotate(const Orientation& o, CubeRot r) {
    Orientation result = o;
    switch (r) {
        case CubeRot::x:
            applyFaceMap(result.faces, X_FACES);
            break;
        case CubeRot::xp:
            applyFaceMap(result.faces, X_FACES);
            applyFaceMap(result.faces, X_FACES);
            applyFaceMap(result.faces, X_FACES);
            break;
        case CubeRot::x2:
            applyFaceMap(result.faces, X_FACES);
            applyFaceMap(result.faces, X_FACES);
            break;
        case CubeRot::y:
            applyFaceMap(result.faces, Y_FACES);
            break;
        case CubeRot::yp:
            applyFaceMap(result.faces, Y_FACES);
            applyFaceMap(result.faces, Y_FACES);
            applyFaceMap(result.faces, Y_FACES);
            break;
        case CubeRot::y2:
            applyFaceMap(result.faces, Y_FACES);
            applyFaceMap(result.faces, Y_FACES);
            break;
        case CubeRot::z:
            applyFaceMap(result.faces, Z_FACES);
            break;
        case CubeRot::zp:
            applyFaceMap(result.faces, Z_FACES);
            applyFaceMap(result.faces, Z_FACES);
            applyFaceMap(result.faces, Z_FACES);
            break;
        case CubeRot::z2:
            applyFaceMap(result.faces, Z_FACES);
            applyFaceMap(result.faces, Z_FACES);
            break;
    }
    return result;
}

Move translateMove(Move m, const Orientation& o) {
    int idx     = static_cast<int>(m);
    int face    = idx / 3;
    int kind    = idx % 3;
    int newFace = o.faces[face];
    return static_cast<Move>(newFace * 3 + kind);
}

SolverState applyMove(const SolverState& s, Move m) {
    SolverState result = s;
    result.cube = s.cube.apply(translateMove(m, s.view));
    return result;
}

SolverState applyRotation(const SolverState& s, CubeRot r) {
    SolverState result = s;
    result.view = rotate(s.view, r);
    return result;
}

// Moves Target Color to Bottom (D position)
SolverState faceToBottom(const SolverState& s, int color) {
    switch (color) {
        case 0: return applyRotation(s, CubeRot::x2);   // white (U) → bottom
        case 1: return applyRotation(s, CubeRot::z);    // red   (R) → bottom
        case 2: return applyRotation(s, CubeRot::xp);   // green (F) → bottom
        case 3: return s;                                // yellow(D) → already bottom
        case 4: return applyRotation(s, CubeRot::zp);   // orange(L) → bottom
        case 5: return applyRotation(s, CubeRot::x);    // blue  (B) → bottom
        default: return s;
    }
}

const char* faceToBottomName(int color) {
    static const char* names[] = {"x2", "z", "x'", "", "z'", "x"};
    return names[color];
}
const char* colorName(int color) {
    static const char* names[] = {"white", "red", "green", "yellow", "orange", "blue"};
    return names[color];
}

std::string orientationName(const Orientation& o) {
    static const char* faces = "URFDLB";
    std::string result;
    for (int i = 0; i < 6; i++) {
        if (i > 0) result += "  ";
        result += faces[i];
        result += ' ';
        result += colorName(o.faces[i]);
    }
    return result;
}
