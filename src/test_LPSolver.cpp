#include <cstdlib>
#include <iostream>

#include "LPSolver.h"

using namespace std;

int main()
{

    vector<double> c {1, 6};
    vector<vector<double>> A {{1, 0}, {0, 1}, {1, 1}};
    vector<double> b {200, 300, 400};
    vector<pair<double, double>> bounds {{0, get_pos_inf(2)}, {0, get_pos_inf(2)}};

    pair<double, vector<double>> res = solve_LP_simplex(c, A, b, bounds);
    if (res.first == 1900)
        cout << "Test succeseded" << endl;
    else
        cout << "Test failed" << endl;

    return EXIT_SUCCESS;
}
