#pragma once

#include <vector>

using namespace std;

typedef enum RETURN_VALUE
{
    NOMEMORY = -2,
    OPTIMAL = 0,
    SUBOPTIMAL = 1,
    INFEASIBLE = 2,
    UNBOUNDED = 3,
    DEGENERATE = 4,
    NUMFAILURE = 5,
    USERABORT = 6,
    TIMEOUT = 7,
    PRESOLVED = 9,
    ACCURACYERROR = 25
} RETURN_VALUE_T;

typedef struct matrix
{

} matrix_t;

matrix_t make_matrix(int rows, int cols, vector<vector<double>> values);

pair<double, vector<double>> solve_LP_simplex(vector<double> c, vector<vector<double>> A, vector<double> b, vector<pair<double, double>> bounds);

double get_pos_inf(int num_varaiables);
