#include "Coords.h"

namespace Coords {

static int choose(int n, int k) {
    if (k < 0 || k > n) return 0;
    if (k == 0 || k == n) return 1;
    int result = 1;
    for (int i = 0; i < k; i++) {
        result *= (n - i);
        result /= (i + 1);
    }
    return result;
}

int encodeFlip(const CubeState& s) {
    return s.eo[0]  * (1 << 10) +
           s.eo[1]  * (1 << 9)  +
           s.eo[2]  * (1 << 8)  +
           s.eo[3]  * (1 << 7)  +
           s.eo[4]  * (1 << 6)  +
           s.eo[5]  * (1 << 5)  +
           s.eo[6]  * (1 << 4)  +
           s.eo[7]  * (1 << 3)  +
           s.eo[8]  * (1 << 2)  +
           s.eo[9]  * (1 << 1)  +
           s.eo[10] * (1 << 0);
}

int encodeTwist(const CubeState& s) {
    return s.co[0] * 729 +
           s.co[1] * 243 +
           s.co[2] * 81  +
           s.co[3] * 27  +
           s.co[4] * 9   +
           s.co[5] * 3   +
           s.co[6] * 1;
}

int encodeSlice(const CubeState& s) {
    int result = 0;
    int k = 3;
    for (int i = 11; i >= 0; i--) {
        if (s.ep[i] >= 8) {
            k--;
        } else {
            result += choose(i, k);
        }
    }
    return result;
}

void decodeFlip(CubeState& s, int flip) {
    int sum = 0;
    for (int i = 10; i >= 0; i--) {
        s.eo[i] = flip % 2;
        sum += s.eo[i];
        flip /= 2;
    }
    s.eo[11] = (sum % 2 == 0) ? 0 : 1;
}

void decodeTwist(CubeState& s, int twist) {
    int sum = 0;
    for (int i = 6; i >= 0; i--) {
        s.co[i] = twist % 3;
        sum += s.co[i];
        twist /= 3;
    }
    s.co[7] = (3 - (sum % 3)) % 3;
}

void decodeSlice(CubeState& s, int slice) {
    int k = 3;
    for (int i = 11; i >= 0; i--) {
        if (slice >= choose(i, k)) {
            slice -= choose(i, k);
            s.ep[i] = 0;
        } else {
            s.ep[i] = 8 + k;
            k--;
        }
    }
}

} // namespace Coords