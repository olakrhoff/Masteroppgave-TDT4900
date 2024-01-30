#include "LPSolver.h"

#include <cstdlib>
#include <iostream>

#include <lp_lib.h>

using namespace std;

matrix_t make_matrix(int rows, int cols, vector<vector<double>> values)
{
    if (rows != values.size())
    {
        cout << "Row dimentions not compatible" << endl;
        exit(EXIT_FAILURE);
    }
    for (auto row : values)
        if (cols != row.size())
        {
            cout << "Col dimentions not compatible" << endl;
            exit(EXIT_FAILURE);
        }

    return matrix_t();
}

double solve_LP_simplex(vector<double> c, vector<vector<double>> A, vector<double> b)
{
    auto num_variables = c.size();
    lprec *lp;
    lp = make_lp(0, num_variables); // num_variables is the number of decision variables
    if (lp == NULL) {
        fprintf(stderr, "Unable to create LP model\n");
        return -1;
    }
    

    // Set the c vector
    REAL objective_coefficients[num_variables];
    for (int i = 0; i < c.size(); ++i)
        objective_coefficients[i] = c.at(i);

    set_obj_fn(lp, objective_coefficients);

    // Add constrint matrix A
    for (int i = 0; i < A.size(); ++i)
    {
        REAL row[num_variables];
        for (int j = 0; j < num_variables; ++j)
            row[j] = A.at(i).at(j);
        auto rhs = b.at(i);
        if (!add_constraint(lp, row, LE, rhs))
        {
            cout << "Failed to add constraint" << endl;
            exit(EXIT_FAILURE);
        }
    }

    // Set bounds for the variables
    for (int i = 0; i < num_variables; ++i)
        set_bounds(lp, i, 0, 1);
    
    int result = solve(lp);

    switch (result)
    {
        case OPTIMAL:
            break;
        default:
            cout << "Got result from LP that is not handled: " << result << endl;
            exit(EXIT_FAILURE);
    }

    //double *solution = (double*)malloc(num_variables * sizeof(*solution));
    //get_variables(lp, solution);
    double objective_value = get_objective(lp);

    delete_lp(lp);

    return objective_value;
}
