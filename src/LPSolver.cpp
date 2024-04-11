#include "LPSolver.h"

#include <cstdlib>
#include <iostream>

#include <lp_lib.h>

using namespace std;

pair<double, vector<double>> solve_LP_simplex(vector<double> c, vector<vector<double>> A, vector<double> b, vector<pair<double, double>> bounds)
{
    int num_variables = c.size();
    lprec *lp;
    // We will build the problem row by row, there for we begin with zero rows
    // and 'num_varaibles' cols
    lp = make_lp(0, num_variables); // num_variables is the number of decision variables
    if (lp == nullptr)
    {
        cout << "Could not create LP model" << endl;
        exit(EXIT_FAILURE);
    }

    set_verbose(lp, NEUTRAL);
   
    // Set the c vector
    REAL objective_coefficients[num_variables + 1];
    objective_coefficients[0] = 0;
    for (int i = 0; i < (int)c.size(); ++i)
        objective_coefficients[i + 1] = c.at(i);

    if (!set_obj_fn(lp, objective_coefficients))
    {
        cout << "Failed to set objective function" << endl;
        exit(EXIT_FAILURE);
    }

    // Add constrint matrix A
    for (int i = 0; i < (int)A.size(); ++i)
    {
        REAL row[num_variables + 1];
        row[0] = 0;
        for (int j = 0; j < num_variables; ++j)
            row[j + 1] = A.at(i).at(j);
        REAL rhs = b.at(i);
        if (!add_constraint(lp, row, LE, rhs))
        {
            cout << "Failed to add constraint" << endl;
            exit(EXIT_FAILURE);
        }
    }

    // Set bounds for the variables
    int index = 1;
    for (auto [lower, upper] : bounds)
        set_bounds(lp, index++, lower, upper);
    
    // We need to explicitly set the problem to a maximisation problem
    set_maxim(lp);

    int result = solve(lp);

    // Prints the LP in a nice format
    //print_lp(lp);
    
    switch (result)
    {
        case OPTIMAL:
            break;
        default:
            cout << "Got result from LP that is not handled: " << result << endl;
            exit(EXIT_FAILURE);
    }

    double solution = get_objective(lp);

    REAL variables[num_variables];

    if (!get_variables(lp, variables))
    {
        cout << "Could not get varaibles from solution" << endl;
        exit(EXIT_FAILURE);
    }


    delete_lp(lp);

    vector<double> vars {};
    for (int i = 0; i < num_variables; ++i)
        vars.emplace_back(variables[i]);

    return {solution, vars};
}

pair<double, vector<double>> solve_ILP(vector<double> c, vector<vector<double>> A, vector<double> b, vector<pair<double, double>> bounds)
{
    int num_variables = c.size();
    lprec *lp;
    // We will build the problem row by row, there for we begin with zero rows
    // and 'num_varaibles' cols
    lp = make_lp(0, num_variables); // num_variables is the number of decision variables
    if (lp == nullptr)
    {
        cout << "Could not create LP model" << endl;
        exit(EXIT_FAILURE);
    }

    set_verbose(lp, NEUTRAL);
   
    // Set the c vector
    REAL objective_coefficients[num_variables + 1];
    objective_coefficients[0] = 0;
    for (int i = 0; i < (int)c.size(); ++i)
        objective_coefficients[i + 1] = c.at(i);

    if (!set_obj_fn(lp, objective_coefficients))
    {
        cout << "Failed to set objective function" << endl;
        exit(EXIT_FAILURE);
    }

    // Add constrint matrix A
    for (int i = 0; i < (int)A.size(); ++i)
    {
        REAL row[num_variables + 1];
        row[0] = 0;
        for (int j = 0; j < num_variables; ++j)
            row[j + 1] = A.at(i).at(j);
        REAL rhs = b.at(i);
        if (!add_constraint(lp, row, LE, rhs))
        {
            cout << "Failed to add constraint" << endl;
            exit(EXIT_FAILURE);
        }
    }

    // Set bounds for the variables
    int index = 1;
    for (auto [lower, upper] : bounds)
        set_bounds(lp, index++, lower, upper);

    // Set all varaibles to be binary
    for (int i = 0; i < num_variables; ++i)
        set_binary(lp, i, TRUE);
    
    // We need to explicitly set the problem to a maximisation problem
    set_maxim(lp);

    int result = solve(lp);

    // Prints the LP in a nice format
    //print_lp(lp);
    
    switch (result)
    {
        case OPTIMAL:
            break;
        default:
            cout << "Got result from LP that is not handled: " << result << endl;
            exit(EXIT_FAILURE);
    }

    double solution = get_objective(lp);

    REAL variables[num_variables];

    if (!get_variables(lp, variables))
    {
        cout << "Could not get varaibles from solution" << endl;
        exit(EXIT_FAILURE);
    }


    delete_lp(lp);

    vector<double> vars {};
    for (int i = 0; i < num_variables; ++i)
        vars.emplace_back(variables[i]);

    return {solution, vars};
}

double get_pos_inf(int num_variables)
{
    lprec *lp;
    lp = make_lp(0, num_variables); // num_variables is the number of decision variables
    if (lp == NULL) {
        fprintf(stderr, "Unable to create LP model\n");
        return -1;
    }

    double inf = get_infinite(lp);

    delete_lp(lp);

    return inf;
}
