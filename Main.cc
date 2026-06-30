#include <iostream>
#include "CubeState/CubeState.h"
#include "CubeState/MoveTable.h"

using namespace std;

int main(int argc, char* argv[]) {
    CubeState c = CubeState::solved();
    cout << c.toString() << endl;
}