#include "Coords.h"

namespace Coords {

// Binomial Coefficient
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

// Phase 1 Coords
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

// Phase 2 Coords
int encodeSlicePerm(const CubeState& s) {
    static const int fact[4] = {1, 1, 2, 6};
    int result = 0;

    for (int i = 8; i < 12; i++) {
        int index = i - 8;
        int curr = s.ep[i];
        int count = 0;
        for (int j = i + 1; j < 12; j++) {
            if (s.ep[j] < curr) {
                count++;
            }
        }

        result += fact[3 - index] * count;
    }
    return result;
}

int encodeCP(const CubeState& s) {
    static const int fact[8] = {1,1,2,6,24,120,720,5040};
    int result = 0;
    for (int i = 0; i < 8; i++) {
        int count = 0;
        for (int j = i + 1; j < 8; j++)
            if (s.cp[j] < s.cp[i]) count++;
        result += count * fact[7 - i];
    }
    return result;
}

int encodeEP(const CubeState& s) {
    static const int fact[8] = {1,1,2,6,24,120,720,5040};
    int result = 0;
    for (int i = 0; i < 8; i++) {
        int count = 0;
        for (int j = i + 1; j < 8; j++)
            if (s.ep[j] < s.ep[i]) count++;
        result += count * fact[7 - i];
    }
    return result;
}

void decodeSlicePerm(CubeState& s, int perm) {
    static const int fact[4] = {1, 1, 2, 6};
    bool used[4] = {false, false, false, false};

    for (int i = 0; i < 4; i++) {
        // Extract Digit for This Position
        int digit = perm / fact[3 - i];
        perm     %= fact[3 - i];

        // Find the Digit-th Unused Value
        int count = 0;
        for (int v = 0; v < 4; v++) {
            if (!used[v]) {
                if (count == digit) {
                    s.ep[8 + i] = 8 + v;
                    used[v] = true;
                    break;
                }
                count++;
            }
        }
    }
}

void decodeCP(CubeState& s, int cp) {
    static const int fact[8] = {1,1,2,6,24,120,720,5040};
    bool used[8] = {};
    for (int i = 0; i < 8; i++) {
        int digit = cp / fact[7 - i];
        cp       %= fact[7 - i];
        int count = 0;
        for (int v = 0; v < 8; v++) {
            if (!used[v]) {
                if (count == digit) {
                    s.cp[i] = v;
                    used[v] = true;
                    break;
                }
                count++;
            }
        }
    }
}

void decodeEP(CubeState& s, int ep) {
    static const int fact[8] = {1,1,2,6,24,120,720,5040};
    bool used[8] = {};
    for (int i = 0; i < 8; i++) {
        int digit = ep / fact[7 - i];
        ep       %= fact[7 - i];
        int count = 0;
        for (int v = 0; v < 8; v++) {
            if (!used[v]) {
                if (count == digit) {
                    s.ep[i] = v;
                    used[v] = true;
                    break;
                }
                count++;
            }
        }
    }
}
} // namespace Coords
