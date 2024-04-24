#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <chrono>
#include <iostream>
#include <fstream>
#include <numeric>
#include <vector>
#include <algorithm>
#include <random>
#include <stack>
#include <float.h>
#include <getopt.h>
#include <utility>

#include "LPSolver.h"
#include "types.h"

using namespace std;

/**
 * This is code that I have reused from my earlier projects, look at my github 
 */
vector<string> split(const string &line, const string &delimiter)
{
    vector<string> result {};

    string temp = line;
    while ((int)temp.find(delimiter) != -1)
    {
        result.emplace_back(temp.substr(0, temp.find(delimiter)));
        temp = temp.substr(temp.find(delimiter) + delimiter.length());
    }
    result.emplace_back(temp);

    return result;
}

typedef enum OPTION_X
{
    GOODS,
    AGENTS,
    N_OVER_M
} OPTION_X_T;

typedef enum OPTION_Y
{
    TIME,
    NODES
} OPTION_Y_T;

typedef enum PICKING_ORDERS
{
    NONE,
    RANDOM,
    NASH,
    MAX_VALUE,
    MAX_WEIGHT,
    MAX_PROFIT,
} PICKING_ORDERS_T;

string data_in_filepath;
string data_out_filepath = "data/analysis";
string RESULTS_EXPORT_PATH = GLOBAL_RESULTS_FILEPATH;
OPTION_X_T x_axis_option {GOODS};
uint64_t x_axis_lower_bound {}, x_axis_upper_bound {1}; 
bool RUN_SINGLE_X {true};
OPTION_Y_T y_axis_option {TIME};
int UPPER_BOUND_MIN_LIMIT {7};
bool EXPORT_RESULT {false};
bool DATA_WRITE_ACTICE {false};

typedef struct options
{
    bool upper_bound {false};
    bool bound_and_bound {false};
    PICKING_ORDERS_T goods_order {NONE};
    PICKING_ORDERS_T agents_order {NONE};
    bool reverse_goods_order {false};
    bool reverse_agents_order {false};
    bool non_naive {false};
    bool mip_solver_active {false};
    bool skip {false};
} options_t;

typedef struct attributes
{
    uint64_t number_of_agents {};
    uint64_t number_of_goods {};
    double avg_permutation_distance {};
    double avg_value_distance {};
    double m_over_n {};
    double budget_cap_percent {};
} attributes_t;

typedef struct configurations
{ 
    attributes_t attributes {};
    options_t options {};
    uint64_t time {};

    friend ostream& operator<<(ostream &os, const configurations &config)
    {
        os << data_in_filepath << ", "
           << config.attributes.number_of_agents << ", "
           << config.attributes.number_of_goods << ", "
           << config.attributes.m_over_n << ", "
           << config.attributes.avg_permutation_distance << ", "
           << config.attributes.avg_value_distance << ", "
           << config.attributes.budget_cap_percent << ", "
           // Separating here for readability
           << config.options.upper_bound << ", "
           << config.options.bound_and_bound << ", "
           << config.options.non_naive << ", "
           << config.options.goods_order << ", "
           << config.options.agents_order << ", "
           << config.options.reverse_goods_order << ", "
           << config.options.reverse_agents_order << ", "
           << (config.options.mip_solver_active ? "1" : "0") << ", "
           << config.time
           << endl;

        return os;
    }

} configurations_t;

configurations_t CONFIGURATION {};

/**
 * This function handles all the options sent into the program.
 * It sets flags for optimisations that are enabled and what type of output
 * is expected to be preinted and to where.
 *
 * -d (data in): the input data filepath
 * -o (data out): the output data filepath
 * -x[from:to] (x-axis): the data for the x-axis
 *      g: GOODS
 *      a: AGENTS
 *      n: N_OVER_M
 * -y (y-axis): the data for the y-axis
 *      t: TIME
 *      n: NODES
 *
 * --- OPTIMISATIONS ---
 *
 * -m (MIP_SOLVER_ACTIVE): actives the use of the MIP solver to just solve the ILP
 *
 * -n (NON_NAIVE): allows a class of optimisations that are significant, but not
 *                 big enough to stand on their own.
 * -u (UPPER_BOUND): takes no argument, simply turns on opt. when given
 * -b (BOUND_AND_BOUND): takes no argument, simply turns on opt. when given
 *                       this will turn on the UPPER_BOUND as well, since it
 *                       is required
 * -p (Picking Order): Selects a picking order for goods or agents, if not
 *                     passed, RANDOM ís the default choice.
 *                     Format: -p <g|a><order>
 *                     r:  RANDOM
 *                     n:  NASH
 *                     v:  MAX_VALUE
 *                     w:  MAX_WEIGHT
 *                     p:  MAX_PROFIT
 * -r (Reverse picking order): No argument, reverses the picking order of goods
 *                             or agents.
 *                             g: GOODS
 *                             a: AGENTS
 *
 * --- OTHER ---
 * 
 * -s (skip solving): Skips the solving, this is just a way to get the attributes
 *                    exported quickly.
 *
 * -e (export results): writes results to the global file of results. Might take
 *                      an argument for the filepath to where to store the data.
 */
void handle_options(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i)
        cout << argv[i] << " : ";
    cout << endl;
    int code {};
    while ((code = getopt(argc, argv, "d:o:x:y:ubp:r:e::nms")) != -1)
    {
        switch (code)
        {
            case 's':
                CONFIGURATION.options.skip = true;
                break;
            case 'm':
                CONFIGURATION.options.mip_solver_active = true;
                break;
            case 'n':
                CONFIGURATION.options.non_naive = true;
                break;
            case 'e':
                EXPORT_RESULT = true;
                if (optind < argc && *argv[optind] != '-')
                {
                    RESULTS_EXPORT_PATH = string(argv[optind]);
                    optind++;
                }
                break;
            case 'r':
                {
                    if (string(optarg).length() != 1)
                    {
                        cout << "Expecting one argument for -r. Expected 'g' or 'a'" << endl;
                        exit(EXIT_FAILURE);
                    }
                    switch (*optarg)
                    {
                        case 'g':
                            CONFIGURATION.options.reverse_goods_order = true;
                            break;
                        case 'a':
                            CONFIGURATION.options.reverse_agents_order = true;
                            break;
                        default:
                            cout << "Could not recognise value given for -r option: " << optarg << endl;
                            exit(EXIT_FAILURE);
                    }
                }
                break;
            case 'p':
                {
                    if (string(optarg).length() != 2)
                    {
                        cout << "Expect two arguements for -p, got: " << string(optarg).length() << endl;
                        cout << "Use format: '-p <g|a><order>', e.g. '-p an' see documentation for overview of orders" << endl;
                        exit(EXIT_FAILURE);
                    }
                    PICKING_ORDERS_T order;
                    switch (*(optarg + 1))
                    {
                        case 'r':
                            order = RANDOM;
                            break;
                        case 'n':
                            order = NASH;
                            break;
                        case 'v':
                            order = MAX_VALUE;
                            break;
                        case 'w':
                            order = MAX_WEIGHT;
                            break;
                        case 'p':
                            order = MAX_PROFIT;
                            break;
                        default:
                            cout << "Picking order arguement: '" << optarg << "' is not recognised" << endl;
                            exit(EXIT_FAILURE);
                    }

                    switch (*optarg)
                    {
                        case 'g':
                            CONFIGURATION.options.goods_order = order;
                            break;
                        case 'a':
                            CONFIGURATION.options.agents_order = order;
                            break;
                        default:
                            cout << "Could not recognise value given for -p option: " << optarg << endl;
                            cout << "The first argument must be 'g' or 'a'" << endl;
                            exit(EXIT_FAILURE);
                    }
                }
                break;
            case 'b':
                CONFIGURATION.options.upper_bound = true;
                CONFIGURATION.options.bound_and_bound = true;
                UPPER_BOUND_MIN_LIMIT = 6;
                break;
            case 'u':
                   CONFIGURATION.options.upper_bound = true;
                break;
            case 'o':
                DATA_WRITE_ACTICE = true;
                data_out_filepath = optarg; 
                break;
            case 'd':
                data_in_filepath = optarg;
                break;
            case 'x':
                {
                    switch (*optarg)
                    {
                        case 'g':
                            x_axis_option = GOODS;
                            break;
                        case 'a':
                            x_axis_option = AGENTS;
                            break;
                        case 'n':
                            x_axis_option = N_OVER_M;
                            break;
                        default:
                            cout << "Did not recognise the argument for -x: '" << optarg << "'" << endl;
                            exit(EXIT_FAILURE);
                    }
                    string temp = optarg;
                    temp = temp.substr(1);
                    if (temp.empty() || temp.length() == 0)
                        break;
                    auto bounds = split(temp, ":");
                    x_axis_lower_bound = stoi(bounds.at(0));
                    x_axis_upper_bound = stoi(bounds.at(1));
                    RUN_SINGLE_X = false;
                    break;
                }
            case 'y':
                switch (*optarg)
                {
                    case 't':
                        y_axis_option = TIME;
                        break;
                    case 'n':
                        y_axis_option = NODES;
                        break;
                    default:
                        cout << "Did not recognise the argument for -y: '" << optarg << "'" << endl;
                        exit(EXIT_FAILURE);
                }
                break;
            default:
                cout << "Parsing error for the options: " << (char)optopt << endl;
                exit(EXIT_FAILURE);
        }
    }
}

void validate_options()
{
    if (data_in_filepath.empty())
    {
        cout << "To run the program you need to pass a data file with the -d <filepath> option" << endl;
        exit(EXIT_FAILURE);
    }

    if (CONFIGURATION.options.mip_solver_active)
    {
        bool other_conditions {false};
        
        if (CONFIGURATION.options.reverse_agents_order)
        {
            other_conditions = true;
            CONFIGURATION.options.reverse_agents_order = false;
        }

        if (CONFIGURATION.options.reverse_goods_order)
        {
            other_conditions = true;
            CONFIGURATION.options.reverse_goods_order = false;
        }
        if (CONFIGURATION.options.agents_order != PICKING_ORDERS_T::NONE)
        {
            other_conditions = true;
            CONFIGURATION.options.agents_order = PICKING_ORDERS_T::NONE;
        }
        if (CONFIGURATION.options.goods_order != PICKING_ORDERS_T::NONE)
        {
            other_conditions = true;
            CONFIGURATION.options.goods_order = PICKING_ORDERS_T::NONE;
        }
        if (CONFIGURATION.options.non_naive)
        {
            other_conditions = true;
            CONFIGURATION.options.non_naive = false;
        }
        if (CONFIGURATION.options.upper_bound)
        {
            other_conditions = true;
            CONFIGURATION.options.upper_bound = false;
        }
        if (CONFIGURATION.options.bound_and_bound)
        {
            other_conditions = true;
            CONFIGURATION.options.bound_and_bound = false;
        }

        if (other_conditions)
            cout << "When -m is active no other arguments for optimisations will be applied" << endl;
    }
}

void write_data_to_file(const vector<uint64_t> &times)
{
    ofstream file;

    switch (x_axis_option)
    {
        case GOODS:
            data_out_filepath += "_GOODS";
            break;
        case AGENTS:
            data_out_filepath += "_AGENTS";
            break;
        case N_OVER_M:
            data_out_filepath += "_NOVERM";
            break;
    }

    switch (y_axis_option)
    {
        case TIME:
            data_out_filepath += "_TIME";
            break;
        case NODES:
            data_out_filepath += "_NODES";
            break;
    }

    if (CONFIGURATION.options.upper_bound)
        data_out_filepath += "_UPPER_BOUND";
    if (CONFIGURATION.options.bound_and_bound)
        data_out_filepath += "_BOUND_AND_BOUND";

    data_out_filepath += ".txt";

    if (DATA_WRITE_ACTICE)
    {
        cout << "SADSLKDØAS D" << endl;
        file.open(data_out_filepath);
        if (!file.is_open())
        {
            cout << "Could not open file: " << data_out_filepath << endl;
            exit(EXIT_FAILURE);
        }


        //Write the timestamp for each of the instances
        //TODO: extend such that we run multiple instances for each case
        for (int i = 0; i < (int)times.size(); ++i)
        {
            // Write all optimisations that is turned on
            //TODO
            file << ": ";
            //Write AGENTS, GOODS, RUNS, NODES:
            file << "3, " << i + x_axis_lower_bound << ", 1, 0: ";
            file << times.at(i) << endl;
        }

        file.close();
    }

    if (EXPORT_RESULT)
    {
        // Here we want to write the result to the global table of results
        // the file path is stored in the types.h file. We don't want to 
        // overwrite the data in the file so we will have to append the data
        // to the end of the file instead
        file.open(RESULTS_EXPORT_PATH, ios::out | ios::app);

        if (!file.is_open())
        {
            cout << "Could not open file to export data to: " << RESULTS_EXPORT_PATH << endl;
            exit(EXIT_FAILURE);
        }

        // TODO: We need to add the attribute values and which optimisations 
        // are active and of course the result of the running
        // Format: "N, M, M/N, AVG_PERM_DIST, AVG_VAL_DIST, BUDGET/SIZE, TIME"
    
        file << CONFIGURATION;

        file.close();
    }
}

typedef struct good
{
    double value {};

    good(double value) : value(value)
    {}
    
    operator double() const
    {
        return value;
    }
} good_t;

typedef struct agent
{
    vector<good_t> goods {};
    uint64_t capacity {};
    
    operator double() const
    {
        return (double)capacity;
    }
} agent_t;

vector<weight_t> parse_line_to_weights(const string &line)
{
    vector<weight_t> result {};
    auto weight_strings = split(line, " ");
    for (auto val : weight_strings)
        result.emplace_back(stoi(val));

    return result;
}

agent_t parse_line_to_values(const string &line)
{
    uint64_t capacity {};

    auto capacity_and_goods = split(line, ": ");
    capacity = stoi(capacity_and_goods.at(0));

    auto temp = capacity_and_goods.at(1);
    vector<good_t> goods {};

    auto string_vals = split(temp, " ");
    for (auto val : string_vals)
        goods.emplace_back(stoi(val));

    return {goods, capacity};
}

typedef struct allocation
{
    private:
    vector<vector<uint64_t>> goods_for_agents {};
    vector<weight_t> weights {};

    public:
    allocation(uint64_t num_agents)
    {
        goods_for_agents.reserve(num_agents);
        vector<uint64_t> temp {};
        for (uint64_t i = 0; i < num_agents; ++i)
        {
            goods_for_agents.emplace_back(temp);
            weights.emplace_back(0);
        }
    }

    vector<uint64_t> get_goods_allocated_to_agent(int agent_idx) const
    {
        try
        {
            return goods_for_agents.at(agent_idx);
        }
        catch(...)
        {
            cout << "Failed to access agent's allocated goods" << endl;
            cout << "The allocation does not have this agent index" << endl;
            cout << "Size: " << goods_for_agents.size() << ", agent index passed: " << agent_idx << endl;
            exit(EXIT_FAILURE);
        }
    }

    weight_t get_weigth_for_agent(int agent_idx) const
    {
        try
        {
            return weights.at(agent_idx);
        }
        catch(...)
        {
            cout << "Failed to access agent's weight" << endl;
            cout << "The allocation does not have this agent index" << endl;
            cout << "Size: " << goods_for_agents.size() << ", agent index passed: " << agent_idx << endl;
            exit(EXIT_FAILURE);
        }
    }

    void allocate_good_to_agent(int good_index, weight_t good_weight, int agent_index)
    {
        try
        {
            goods_for_agents.at(agent_index).emplace_back(good_index);
            weights.at(agent_index) += good_weight;
        }
        catch(...)
        {
            cout << "Failed to allocate good to agent" << endl;
            exit(EXIT_FAILURE);
        }
    }

    weight_t get_agents_weight(int agent_idx) const
    {
        try
        {
            return weights.at(agent_idx);
        }
        catch(...)
        {
            cout << "Failed to get agents weight" << endl;
            exit(EXIT_FAILURE);
        }
    }

    int get_number_of_agents() const
    {
        return goods_for_agents.size();
    }
} allocation_t;

typedef struct state
{
    private:
    allocation_t allocation;
    vector<uint64_t> charity {};
    uint64_t num_goods_allocated {};

    public:
    state(uint64_t num_agents) : allocation(num_agents)
    {}

    void allocate_good_to_agent(uint64_t good_index, uint64_t agent_index, uint64_t goods_weight)
    {
        allocation.allocate_good_to_agent(good_index, goods_weight, agent_index);
        num_goods_allocated++;
    }

    void allocate_to_charity(uint64_t good_index)
    {
        charity.emplace_back(good_index);
        num_goods_allocated++;
    }

    weight_t get_agents_weight(const uint64_t agent_index)
    {
        return allocation.get_agents_weight(agent_index);
    }

    double get_agents_value(const uint64_t agent_index, const agent_t &agent)
    {
        auto goods = allocation.get_goods_allocated_to_agent(agent_index);
        double val {0.0};

        for (auto good_idx : goods)
            val += agent.goods.at(good_idx).value;

        return val;
    }
    
    uint64_t get_goods_allocated() const
    {
        return num_goods_allocated;
    }

    allocation_t get_allocation() const
    {
        return allocation;
    }
} state_t;

/**
 * This function is suppose to take an allocation and find the value of the
 * allocation based on the value functions.
 *
 * This is now using a minimisation cirterion, but it is not quite MMS yet
 */
double alpha_MMS_of_allocation(const allocation_t &allocation, 
        const vector<agent_t> &valuation,
        const vector<double> &agent_MMS)
{
    double value = DBL_MAX;
    for (int i = 0; i < (int)allocation.get_number_of_agents(); ++i)
    {
        double temp {};
        for (auto good : allocation.get_goods_allocated_to_agent(i))
            temp += valuation.at(i).goods.at(good).value;
        temp /= agent_MMS.at(i);
        if (temp < value)
            value = temp;
    }
    return value;
}

double min_bundle(const allocation_t &allocation,
        const vector<agent_t> &valuation)
{
    double value = DBL_MAX;
    for (int agent_idx = 0; agent_idx < (int)allocation.get_number_of_agents(); ++agent_idx)
    {
        double temp {0};
        for (auto good_idx : allocation.get_goods_allocated_to_agent(agent_idx))
            temp += valuation.at(agent_idx).goods.at(good_idx).value;

        if (temp < value)
            value = temp;
    }
    return value;
}

/**
 * This function is only for use by other functions, it solves the ILP
 * given to it
 */
pair<allocation_t, double> solve_MIP_simplex(const vector<agent_t> &agents, const vector<weight_t> &weights)
{
    int num_agents = (int)agents.size();
    int num_goods = (int)weights.size();

    auto num_variables = num_agents * num_goods + 1;
 
    vector<double> c {};
    for (int i = 0; i < (int)num_variables - 1; ++i)
        c.emplace_back(0);
    c.emplace_back(1);

    vector<vector<double>> A {};
    vector<double> zeros {};
    for (int i = 0; i < (int)num_variables; ++i)
        zeros.emplace_back(0);
    
    // Add all the budget constraints
    for (int i = 0; i < (int)num_agents; ++i)
    {
        auto temp = zeros;

        auto offset = i * num_goods;
        for (int j = 0; j < (int)num_goods; ++j)
            temp.at(j + offset) = weights.at(j);

        A.emplace_back(temp);
    }
    // Add the negative normalised value and a one in z 
    for (int i = 0; i < (int)num_agents; ++i)
    {
        auto temp = zeros;

        auto offset = i * num_goods;
        for (int j = 0; j < (int)num_goods; ++j)
            temp.at(j + offset) = -agents.at(i).goods.at(j).value;
        temp.back() = 1;

        A.emplace_back(temp);
    }
    // Add constraints such that a good is only allocated at most once
    for (int i = 0; i < (int)num_goods; ++i)
    {
        auto temp = zeros;
        for (int j = 0; j < (int)num_agents; ++j)
            temp.at((j * num_goods) + i) = 1;

        A.emplace_back(temp);
    }   
    vector<double> b {};
    // Add all the budgets to the b vector
    for (int i = 0; i < (int)num_agents; ++i)
    {
        b.emplace_back(agents.at(i).capacity);
    }
    // Add equally many zeros
    for (int i = 0; i < (int)num_agents; ++i)
        b.emplace_back(0);
    // Add equally many ones
    for (int i = 0; i < (int)num_goods; ++i)
        b.emplace_back(1);


    vector<pair<double, double>> bounds {};
    for (int i = 0; i < (int)num_variables - 1; ++i)
        bounds.emplace_back(0, 1);
    bounds.emplace_back(0, get_pos_inf(num_variables));

    double MMS {};
    vector<double> variables {};
    tie(MMS, variables) = solve_ILP(c, A, b, bounds); 

    state_t solved_state(num_agents);

    for (int agent_idx = 0; agent_idx < num_agents; ++agent_idx)
        for (int good_idx = 0; good_idx < num_goods; ++good_idx)
            if (variables.at(agent_idx * num_goods + good_idx) == 1)
                solved_state.allocate_good_to_agent(good_idx, agent_idx, weights.at(good_idx));

    return {solved_state.get_allocation(), MMS};
}

vector<agent_t> mirror_agents(const vector<agent_t> &agents, const int agent_idx)
{
    vector<agent_t> mirror_agents {};

    for (int i = 0; i < (int)agents.size(); ++i)
        mirror_agents.emplace_back(agents.at(agent_idx));

    return mirror_agents;
}

pair<allocation_t, double> solve_MIP(const vector<agent_t> &agents, const vector<weight_t> &weights)
{
    int num_agents = (int)agents.size();
    int num_goods = (int)weights.size();

    vector<double> agents_MMS {};
    for (int agent_idx = 0; agent_idx < num_agents; ++agent_idx)
    {
        auto mirror_agent = mirror_agents(agents, agent_idx);
        agents_MMS.emplace_back(solve_MIP_simplex(mirror_agent, weights).second);
        cout << "Found MMS for agent " << agent_idx <<  ": " << agents_MMS.back() << endl;  

        if (agents_MMS.back() == 0)
        {
            auto reduced_agents = agents;
            reduced_agents.erase(reduced_agents.begin() + agent_idx);
            return solve_MIP(reduced_agents, weights);
        }
    }

    cout << "MMS found for all agents" << endl;

    vector<agent_t> new_agents {};

    for (int agent_idx = 0; agent_idx < (int)agents.size(); ++agent_idx)
    {
        new_agents.emplace_back(agents.at(agent_idx));
        for (int good_idx = 0; good_idx < num_goods; ++good_idx)
            new_agents.back().goods.at(good_idx).value /= agents_MMS.at(agent_idx);
    }

    allocation_t allocation(num_agents);
    double alpha_MMS {};
    tie(allocation, alpha_MMS) = solve_MIP_simplex(new_agents, weights); 

    return {allocation, alpha_MMS};
}

tuple<double, double, state_t, bool> upper_bound(vector<agent_t> agents, vector<weight_t> weights,
                   const state_t &current_state)
{
    int num_agents = (int)agents.size();
    int num_goods = (int)weights.size();
    
    auto num_variables = num_agents * num_goods + 1;
    
    vector<double> c {};
    for (int i = 0; i < (int)num_variables - 1; ++i)
        c.emplace_back(0);
    c.emplace_back(1);

    vector<vector<double>> A {};
    vector<double> zeros {};
    for (int i = 0; i < (int)num_variables; ++i)
        zeros.emplace_back(0);
    
    // Add all the budget constraints
    for (int i = 0; i < (int)num_agents; ++i)
    {
        auto temp = zeros;

        auto offset = i * num_goods;
        for (int j = 0; j < (int)num_goods; ++j)
            temp.at(j + offset) = weights.at(j);

        A.emplace_back(temp);
    }
    // Add the negative normalised value and a one in z 
    for (int i = 0; i < (int)num_agents; ++i)
    {
        auto temp = zeros;

        auto offset = i * num_goods;
        for (int j = 0; j < (int)num_goods; ++j)
            temp.at(j + offset) = -agents.at(i).goods.at(j).value;
        temp.back() = 1;

        A.emplace_back(temp);
    }
    // Add constraints such that a good is only allocated at most once
    for (int i = 0; i < (int)num_goods; ++i)
    {
        auto temp = zeros;
        for (int j = 0; j < (int)num_agents; ++j)
            temp.at((j * num_goods) + i) = 1;

        A.emplace_back(temp);
    }   
    vector<double> b {};
    // Add all the budgets to the b vector
    for (int i = 0; i < (int)num_agents; ++i)
        b.emplace_back(agents.at(i).capacity);
    // Add equally many zeros
    for (int i = 0; i < (int)num_agents; ++i)
        b.emplace_back(0);
    // Add equally many ones
    for (int i = 0; i < (int)num_goods; ++i)
        b.emplace_back(1);


    vector<pair<double, double>> bounds {};
    for (int i = 0; i < (int)num_variables - 1; ++i)
        bounds.emplace_back(0, 1);
    bounds.emplace_back(0, get_pos_inf(num_variables));

    // Now we set all the already allocated goods bounds to be tight
    for (int i = 0; i < (int)num_agents; ++i)
        for (int j = 0; j < (int)current_state.get_allocation().get_goods_allocated_to_agent(i).size(); ++j)
        {
            auto good_idx = current_state.get_allocation().get_goods_allocated_to_agent(i).at(j);
            bounds.at(good_idx + i * num_goods) = {1, 1};
        }
    double upper_bound {};
    vector<double> variables {};
    tie(upper_bound, variables) = solve_LP_simplex(c, A, b, bounds); 

    state_t state = current_state;
    bool tight_bound {false};
    double lower_bound {-1};

    // --- HANDLE LOWER BOUND ---
    if (CONFIGURATION.options.bound_and_bound)
    {
        // The entire premis of the lower bound is to make the upper bound a
        // feasible solution which will then work as our lower bound
        for (int j = 0; j < num_goods; ++j)
        {
            int agent_idx {};
            double value {};
            // First identifiy the agent whose gonna get the good
            for (int i = 0; i < num_agents; ++i)
            {
                auto var = variables.at(i * num_goods + j);
                if (var > value)
                {
                    agent_idx = i;
                    value = var;
                }
            }
            for (int i = 0; i < num_agents; ++i)
            {
                auto &var = variables.at(i * num_goods + j);
                if (i == agent_idx)
                    var = 1;
                else
                    var = 0;
            }
        }
        
        // Now that we have rounded the LP to an integer answer we must check
        // if it is feasible, if not, we need to reduce the allocations until
        // it is. 
        state_t lower_bound_state(num_agents);
        for (int agent_idx = 0; agent_idx < num_agents; ++agent_idx)
        {
            for (int good_idx = 0; good_idx < num_goods; ++good_idx)
            {
                if (variables.at(good_idx + agent_idx * num_goods) == 0)
                    continue;

                if (lower_bound_state.get_agents_weight(agent_idx) + weights.at(good_idx) <= agents.at(agent_idx).capacity)
                    lower_bound_state.allocate_good_to_agent(good_idx, agent_idx, weights.at(good_idx));
                else
                    lower_bound_state.allocate_to_charity(good_idx);
            }
        }


        // This funciton is called with modified values, such that if we want
        // to find the alpha-MMS the values have already been adjusted when we
        // call this method
        lower_bound = min_bundle(lower_bound_state.get_allocation(), agents);
        
        // If the lower bound and upper bound are the same, then we have a tight
        // solution and we can say that this will be the answer of the subproblem
        // meaning we can prune the whole search of the tree below it
        state = lower_bound_state;
        if (lower_bound == upper_bound)
            tight_bound = true;
    }
    
    return {upper_bound, lower_bound, state, tight_bound};
}

tuple<double, double, state_t, bool> upper_bound_with_MMS(vector<agent_t> agents, const vector<weight_t> &weights,
                            vector<double> MMS, const state_t &current_state)
{
    for (int i = 0; i < (int)agents.size(); ++i)
        for (int j = 0; j < (int)agents.at(i).goods.size(); ++j)
            agents.at(i).goods.at(j).value /= MMS.at(i);

    return upper_bound(agents, weights, current_state);
}

tuple<double, double, state_t, bool> upper_bound_find_MMS(const agent_t &agent, const vector<weight_t> &weights,
                            const state_t &current_state)
{
    vector<agent_t> agents {};
    for (int i = 0; i < (int)current_state.get_allocation().get_number_of_agents(); ++i)
        agents.emplace_back(agent);

    return upper_bound(agents, weights, current_state);
}

/**
 * This function is finds the MMS for an agent, given it's value function
 * and the number of agents in the mix
 */
double find_MMS(const agent_t &agent, uint64_t num_agents, const vector<weight_t> &weights)
{
    uint64_t num_goods = agent.goods.size();


    // --- PREPROCESS THE BEST SOLUTION ---
    vector<agent_t> agents {};
    for (int i = 0; i < (int)num_agents; ++i)
        agents.emplace_back(agent);

    double value_of_best_solution {};
    
    if (CONFIGURATION.options.non_naive)
    {
        vector<pair<good_t, weight_t>> goods {};
        for (int i = 0; i < (int)weights.size(); ++i)
            goods.emplace_back(agent.goods.at(i), weights.at(i));

        sort(goods.begin(), goods.end(), [](auto a, auto b){ return a.first.value > b.first.value; });
        auto capacity {agent.capacity};
        for (int i = num_agents - 1; i < (int)goods.size(); i += num_agents)
        {
            auto good {goods.at(i)};
            if (capacity < good.second)
                continue;
            capacity -= good.second;
            value_of_best_solution += good.first.value;
        }
    }

    stack<state_t> state_stack {};
    state_t start_state(num_agents);
    state_stack.push(start_state);

    state_t best_solution_yet = start_state;

    double proportional_value = (double)accumulate(agent.goods.begin(), agent.goods.end(), 0) / num_agents;

    while (!state_stack.empty())
    {
        auto current_state = state_stack.top();
        state_stack.pop();

        // If we have allocated all the goods we now need to evaluate the
        // allocation and score it
        if (current_state.get_goods_allocated() == num_goods)
        {
            double value = min_bundle(current_state.get_allocation(), 
                    agents);
            if (value > value_of_best_solution)
            {
                value_of_best_solution = value;
                best_solution_yet = current_state;
            }
            continue;
        }


        // If an agent gets more than the proportional share there will
        // exists an agent which gets less, thus there is no point in
        // exploring this case
        if (CONFIGURATION.options.non_naive)
            if (current_state.get_agents_value(0, agent) > proportional_value)
                continue;

        double upper_bound {}, lower_bound {};
        state_t state(num_goods * num_agents);
        bool bound {};
        // --- CHECK UPPER BOUND ---
        if (CONFIGURATION.options.upper_bound && num_goods - current_state.get_goods_allocated() >= (uint64_t)UPPER_BOUND_MIN_LIMIT)
        {
            tie(upper_bound, lower_bound, state, bound) = upper_bound_find_MMS(agent, weights, current_state);
            if (upper_bound <= value_of_best_solution)
                continue; // If the upper bound is less than our best solution thus
            if (CONFIGURATION.options.bound_and_bound)
            {
                if (bound || lower_bound > value_of_best_solution)
                {
                    best_solution_yet = state;
                    value_of_best_solution = lower_bound;
                    state_stack.push(current_state);
                    continue;
                }
            }
        }


        // Loop over for each agent and add to the stack the state in which
        // the given agent gets the new good
        for (int i = 0; i < (int)num_agents; ++i)
        {
            auto new_state = current_state;
            uint64_t new_good = current_state.get_goods_allocated();
            if (new_state.get_agents_weight(i) + weights.at(new_good)
                    <= agent.capacity)
            {
                new_state.allocate_good_to_agent(new_good, i, weights.at(new_good));
                
                state_stack.push(new_state);
            }
        }
        // Add the case where the good goes to charity as well, this needs to
        // be taken into account
        auto new_state = current_state;
        uint64_t new_good = current_state.get_goods_allocated();
        new_state.allocate_to_charity(new_good);
        state_stack.push(new_state);
    }

    return value_of_best_solution;
}

template <typename T>
double euclidean_distance(const vector<T> &vec)
{
    double sum {};

    for (auto val : vec)
        sum += pow((double)val, 2);

    return pow(sum, 0.5);
}

double value_distance(const agent_t &value_a, const agent_t &value_b)
{
    int num_goods = (int)value_a.goods.size();
    double dot_product {};

    for (int i = 0; i < num_goods; ++i)
        dot_product += value_a.goods.at(i).value * value_b.goods.at(i).value;

    double length = euclidean_distance(value_a.goods) * euclidean_distance(value_b.goods);

    try
    {
        return acos(dot_product / length);
    }
    catch (...)
    {
        cout << "Could not calculate cos-1(" << dot_product / length << ")" << endl;
        exit(EXIT_FAILURE);
    }
}

double find_avg_value_distance(const vector<agent_t> &agents)
{
    double sum_distance {};
    for (int i = 0; i < (int)agents.size(); ++i)
        for (int j = i + 1; j < (int)agents.size(); ++j)
            sum_distance += value_distance(agents.at(i), agents.at(j));
    
    double n = agents.size();
    double num_distances = (n * (n - 1)) / 2; // Just some maths to explicitly find the number of pairs

    return sum_distance / num_distances;
}

vector<int> get_permutation(const agent_t &perm)
{
    int num_goods = (int)perm.goods.size();
    vector<int> permutation(num_goods, -1);

    auto vals = perm.goods;

    for (int i = 0; i < num_goods; ++i)
    {
        int max_index = 0;
        auto max_value = vals.at(max_index).value;
        for (int j = 0; j < num_goods; ++j)
        {
            auto val = vals.at(j).value;
            if (val > max_value)
            {
                max_value = val;
                max_index = j;
            }
        }
        permutation.at(max_index) = i;
        vals.at(max_index).value = -1;
    }

    return permutation;
}

bool is_pair_in_order(const int left, const int right, const vector<int> &perm)
{
    bool left_found {false};

    for (auto val : perm)
    {
        if (!left_found)
        {
            if (val == left)
                left_found = true;
        }
        else
        {
            if (val == right)
                return true;
        }
    }

    return false;
}

/**
 * This calculates the Kendall Tau distance between two permutations
 */
double permutation_distance(const agent_t &perm_a, const agent_t &perm_b)
{
    int num_goods = (int)perm_a.goods.size();
    bool perm_a_ordered {false}, perm_b_ordered {false};

    vector<int> permutation_a = get_permutation(perm_a);
    vector<int> permutation_b = get_permutation(perm_b);

    double kendall_tau {};
    for (int i = 0; i < num_goods; ++i)
    {
        for (int j = i + 1; j < num_goods; ++j)
        {
            // We now have a pair <i, j>
            perm_a_ordered = is_pair_in_order(i, j, permutation_a);
            perm_b_ordered = is_pair_in_order(i, j, permutation_b);

            if (perm_a_ordered != perm_b_ordered)
                kendall_tau++;
        }
    }

    return kendall_tau;
}

double find_avg_permutation_distance(const vector<agent_t> &agents) 
{
    double sum_distance {};
    for (int i = 0; i < (int)agents.size(); ++i)
        for (int j = i + 1; j < (int)agents.size(); ++j)
            sum_distance += permutation_distance(agents.at(i), agents.at(j));
    
    double n = agents.size();
    double num_distances = (n * (n - 1)) / 2; // Just some maths to explicitly find the number of pairs
                                              
    return sum_distance / num_distances;
}



double get_budget_cap_percent(const vector<agent_t> &agents, const vector<weight_t> &weights)
{
    auto myFunc = [](agent_t a, agent_t b)
    {
        agent_t t;
        t.capacity = a.capacity + b.capacity;
        return t;
    };


    double total_weight = accumulate(weights.begin(), weights.end(), 0);
    double total_budget = accumulate(agents.begin(), agents.end(), agent_t(), myFunc).capacity;

    return total_weight / total_budget;
}

/**
 * This function calculates the different attributes the problem instance has.
 * These are used to make decisions about optimisations and is part of the 
 * general output format.
 */
void find_attributes(const vector<agent_t> &agents, const vector<weight_t> &weights)
{
   CONFIGURATION.attributes.m_over_n = (double)weights.size() / (double)agents.size();
   CONFIGURATION.attributes.number_of_goods = (uint64_t)weights.size();
   CONFIGURATION.attributes.number_of_agents = (uint64_t)agents.size();
   CONFIGURATION.attributes.avg_value_distance = find_avg_value_distance(agents);
   CONFIGURATION.attributes.avg_permutation_distance = find_avg_permutation_distance(agents);
   CONFIGURATION.attributes.budget_cap_percent = get_budget_cap_percent(agents, weights);
}

/*
 * This functions returns the picking order of the goods based on which order has
 * been choosen in the command arguments.
 */
vector<uint64_t> get_picking_order(const vector<agent_t> &agents, const vector<weight_t> &weights)
{
    int num_goods = weights.size();
    int num_agents = agents.size();
    vector<uint64_t> picking_order {};

    switch (CONFIGURATION.options.goods_order)
    {
        case NONE:
            {
                static vector<uint64_t> none_order_goods {};

                if (none_order_goods.empty())
                    for (int i = 0; i < num_goods; ++i)
                        none_order_goods.push_back(i);

                picking_order = none_order_goods;
            }
            break;
        case RANDOM: 
            {
                for (int i = 0; i < (int)num_goods; ++i)
                    picking_order.push_back(i);

                // Since we don't have any picking order optimisation set yet we do a
                // little random shuffle of the picking order to get some more rounded data
                random_device rd;
                mt19937 g(rd());
                shuffle(picking_order.begin(), picking_order.end(), g);
                break;
            }
        case NASH:
            {
                // The pair holds the nash score and the index of the good
                vector<pair<int, int>> ordering {};
                for (int good_idx = 0; good_idx < num_goods; ++good_idx)
                {
                    ordering.emplace_back(1, good_idx);
                    for (int agent_idx = 0; agent_idx < num_agents; ++agent_idx)
                    {
                        int valuation = agents.at(agent_idx).goods.at(good_idx).value;
                        // We ignore zero values 
                        if (valuation == 0)
                            continue;
                        ordering.back().first *= valuation;
                    }
                }       

                // Sort them from highest to lowest Nash score
                sort(ordering.begin(), ordering.end(), [](auto a, auto b){ return a.first > b.first; });
                for (auto [nash, idx] : ordering)
                    picking_order.emplace_back(idx);
                break;
            }
        case MAX_VALUE:
            {
                // Here we calculate the average value for each good and order
                // them based on that.
                // The pair holds the value and the index of the good
                vector<pair<double, int>> ordering {};
                for (int good_idx = 0; good_idx < num_goods; ++good_idx)
                {
                    ordering.emplace_back(0, good_idx);
                    for (int agent_idx = 0; agent_idx < num_agents; ++agent_idx)
                    {
                        int valuation = agents.at(agent_idx).goods.at(good_idx).value;
                        ordering.back().first += valuation;
                    }
                    ordering.back().first /= num_agents;
                }       

                // Sort them from highest to lowest avg. value
                sort(ordering.begin(), ordering.end(), [](auto a, auto b){ return a.first > b.first; });
                for (auto [nash, idx] : ordering)
                    picking_order.emplace_back(idx);

                break;
            }
        case MAX_WEIGHT:
            {
                // The pair holds the weight and the index of the good
                vector<pair<double, int>> ordering {};
                for (int good_idx = 0; good_idx < num_goods; ++good_idx)
                    ordering.emplace_back(weights.at(good_idx), good_idx);

                // Sort them from highest to lowest weight
                sort(ordering.begin(), ordering.end(), [](auto a, auto b){ return a.first > b.first; });
                for (auto [nash, idx] : ordering)
                    picking_order.emplace_back(idx);
                
                break;
            }
        case MAX_PROFIT:
            {
                // Here we calculate the average profit for each good and order
                // them based on that.
                // The pair holds the avg. profit and the index of the good
                vector<pair<double, int>> ordering {};
                for (int good_idx = 0; good_idx < num_goods; ++good_idx)
                {
                    ordering.emplace_back(0, good_idx);
                    for (int agent_idx = 0; agent_idx < num_agents; ++agent_idx)
                    {
                        int valuation = agents.at(agent_idx).goods.at(good_idx).value;
                        ordering.back().first += (double)valuation / weights.at(good_idx);
                    }
                    ordering.back().first /= num_agents;
                }       

                // Sort them from highest to lowest avg. value
                sort(ordering.begin(), ordering.end(), [](auto a, auto b){ return a.first > b.first; });
                for (auto [nash, idx] : ordering)
                    picking_order.emplace_back(idx);

                break;
            }
       default: 
            cout << "The handling of picking order is not implemented yet: " << CONFIGURATION.options.goods_order << endl;
            exit(EXIT_FAILURE);
    }

    // If we are suppose to reverse the order we do so
    if (CONFIGURATION.options.reverse_goods_order)
        reverse(picking_order.begin(), picking_order.end());

    return picking_order;
}

/**
 * This function takes a state and orders the agents picking order based on a
 * criterion. The agents are orderer from least to most in agreement with the criterion.
 */
vector<uint64_t> get_agents_order(const state_t &state, const vector<agent_t> &agents, const vector<weight_t> &weights)
{
    int num_agents = (int)agents.size();
    vector<uint64_t> picking_order {};

    switch (CONFIGURATION.options.agents_order)
    {
        case NONE:
            {
                static vector<uint64_t> none_order_agents {};

                if (none_order_agents.empty())
                    for (int i = 0; i < (int)num_agents; ++i)
                        none_order_agents.push_back(i);

                picking_order = none_order_agents;
            }
            break;
        case RANDOM: 
            {
                for (int i = 0; i < num_agents; ++i)
                    picking_order.push_back(i);

                random_device rd;
                mt19937 g(rd());
                shuffle(picking_order.begin(), picking_order.end(), g);
                break;
            }
        case NASH:
            {
                // The pair holds the nash score and the index of the agent
                vector<pair<double, int>> ordering {};

                for (int agent_idx = 0; agent_idx < num_agents; ++agent_idx)
                {
                    ordering.emplace_back(1, agent_idx);
                    auto bundle = state.get_allocation().get_goods_allocated_to_agent(agent_idx);
                    
                    for (int agent_idx_inner = 0; agent_idx_inner < num_agents; ++agent_idx_inner)
                    {
                        double value_of_bundle {0};
                        for (auto good_idx : bundle)
                            value_of_bundle += agents.at(agent_idx_inner).goods.at(good_idx).value;

                        ordering.back().first *= value_of_bundle;
                    }
                }

                // Sort them from lowest to highest Nash score
                sort(ordering.begin(), ordering.end(), [](auto a, auto b){ return a.first < b.first; });
                for (auto [nash, idx] : ordering)
                    picking_order.emplace_back(idx);
                break;
            }
        case MAX_VALUE:
            {
                // Here we calculate the average value for each good and order
                // them based on that.
                // The pair holds the value and the index of the good
                vector<pair<double, int>> ordering {};

                for (int agent_idx = 0; agent_idx < num_agents; ++agent_idx)
                {
                    ordering.emplace_back(0, agent_idx);
                    auto bundle = state.get_allocation().get_goods_allocated_to_agent(agent_idx);
                    
                    for (int agent_idx_inner = 0; agent_idx_inner < num_agents; ++agent_idx_inner)
                    {
                        double value_of_bundle {0};
                        for (auto good_idx : bundle)
                            value_of_bundle += agents.at(agent_idx_inner).goods.at(good_idx).value;

                        ordering.back().first += value_of_bundle;
                    }
                    ordering.back().first /= num_agents;
                }

                // Sort them from lowest to highest Max score
                sort(ordering.begin(), ordering.end(), [](auto a, auto b){ return a.first < b.first; });
                for (auto [nash, idx] : ordering)
                    picking_order.emplace_back(idx);

                break;
            }
        case MAX_WEIGHT:
            {
                // The pair holds the weight and the index of the agent
                vector<pair<double, int>> ordering {};
                for (int agent_idx = 0; agent_idx < num_agents; ++agent_idx)
                {
                    ordering.emplace_back(0, agent_idx);
                    auto bundle = state.get_allocation().get_goods_allocated_to_agent(agent_idx);
                    
                    for (auto good_idx : bundle)
                        ordering.back().first += weights.at(good_idx);
                }

                // Sort them from lowest to highest weight
                sort(ordering.begin(), ordering.end(), [](auto a, auto b){ return a.first < b.first; });
                for (auto [nash, idx] : ordering)
                    picking_order.emplace_back(idx);
                
                break;
            }
        case MAX_PROFIT:
            {
                // Here we calculate the average profit for each good and order
                // them based on that.
                // The pair holds the avg. profit and the index of the good
                vector<pair<double, int>> ordering {};

                for (int agent_idx = 0; agent_idx < num_agents; ++agent_idx)
                {
                    ordering.emplace_back(0, agent_idx);
                    auto bundle = state.get_allocation().get_goods_allocated_to_agent(agent_idx);
                    
                    for (int agent_idx_inner = 0; agent_idx_inner < num_agents; ++agent_idx_inner)
                    {
                        double value_of_bundle {0};
                        weight_t weight_of_bundle {};
                        for (auto good_idx : bundle)
                        {
                            value_of_bundle += agents.at(agent_idx_inner).goods.at(good_idx).value;
                            weight_of_bundle += weights.at(good_idx);
                        }
                        
                        // Ensures that we don't get a divid by zero and that
                        if (weight_of_bundle == 0)
                            weight_of_bundle = 1;

                        ordering.back().first += value_of_bundle / weight_of_bundle;
                    }
                    ordering.back().first /= num_agents;
                }

                // Sort them from lowest to highest Max score
                sort(ordering.begin(), ordering.end(), [](auto a, auto b){ return a.first < b.first; });
                for (auto [nash, idx] : ordering)
                    picking_order.emplace_back(idx);
                
                break;
            }
       default: 
            cout << "The handling of picking order is not implemented yet: " << CONFIGURATION.options.agents_order << endl;
            exit(EXIT_FAILURE);
    }

    // If we are suppose to reverse the order we do so
    if (CONFIGURATION.options.reverse_agents_order)
        reverse(picking_order.begin(), picking_order.end());

    return picking_order;
}

/**
 * This is the main funtion for the algorithm
 * Here we will run the B&B algorithm to solve the fair allocation of
 * budget symmetry illustion maximin share (BSIMMS)
 */
pair<allocation_t, double> BBCMMS(const vector<agent_t> &agents, 
                                              const vector<weight_t> &weights)
{
    double nodes_visited {};
    int num_goods = (int)agents.at(0).goods.size();
    int num_agents = (int)agents.size();
    
    vector<double> agents_MMS {};
    for (int agent_idx = 0; agent_idx < (int)num_agents; ++agent_idx)
    {
        cout << "Finding MMS for agent " << agent_idx + 1 << endl;
        agents_MMS.emplace_back(find_MMS(agents.at(agent_idx), num_agents, weights));

        // --- HANDLE THE CASE WHERE WE GET A ZERO MMS VALUE ---
        // We simply remove the first one that got a zero value and solve without it
        if (agents_MMS.back() == 0)
        {
            auto reduced_agents = agents;
            auto index = agents_MMS.size() - 1;
            reduced_agents.erase(reduced_agents.begin() + index);
            
            allocation_t allocation(num_agents - 1);
            double alpha_MMS {};

            tie(allocation, alpha_MMS) = BBCMMS(reduced_agents, weights);
           
            allocation_t corrected_allocation(num_agents);
            for (int i = 0; i < agent_idx; ++i)
            {
                for (auto good_idx : allocation.get_goods_allocated_to_agent(i))
                    corrected_allocation.allocate_good_to_agent(good_idx, weights.at(good_idx), i);
            }
            for (int i = agent_idx + 1; i < num_agents; ++i)
            {
                for (auto good_idx : allocation.get_goods_allocated_to_agent(i - 1))
                    corrected_allocation.allocate_good_to_agent(good_idx, weights.at(good_idx), i);
            }

            return {corrected_allocation, alpha_MMS};
        }
        cout << "MMS for agent " << agent_idx + 1 << " is: " << agents_MMS.back() << endl;
    }
    cout << "MMS found for all agents" << endl;




    // --- SETTING THE PICKING ORDER ---

    vector<uint64_t> picking_order_goods = get_picking_order(agents, weights);


    // --- HANDLE PREPROCESSING OF OPTIMISATIONS ---

    // --- BEGIN TRAVERSING THE SOLUTION SPACE ---
    // We want to traverse the search space in a depth-first manner
    // Therefore we create a stack to keep track of where we are in
    // the current state

    stack<state_t> state_stack {};
    state_t start_state(num_agents);
    state_stack.push(start_state);

    state_t best_solution_yet = start_state;

    // It is proven that the BSIMMS has at least a 1/3-MMS solution 
    // It has been proven by Halvard Hummel, just now, that 1/2-MMS exists
    double value_of_best_solution {0.5};
    while (!state_stack.empty())
    {
        nodes_visited++;

        auto current_state = state_stack.top();
        state_stack.pop();

        // If we have allocated all the goods we now need to evaluate the
        // allocation and score it
        if ((int)current_state.get_goods_allocated() == num_goods)
        {
            double value = alpha_MMS_of_allocation(current_state.get_allocation(),
                    agents, agents_MMS);
            if (value > value_of_best_solution)
            {
                value_of_best_solution = value;
                best_solution_yet = current_state;
            }
            continue;
        }

        // --- CHECK UPPER BOUND ---
        if (CONFIGURATION.options.upper_bound && num_goods - current_state.get_goods_allocated() >= (uint64_t)UPPER_BOUND_MIN_LIMIT)
        {
            double upper_bound {}, lower_bound {};
            state_t state(num_goods * num_agents);
            bool bound {};
            tie(upper_bound, lower_bound, state, bound) = upper_bound_with_MMS(agents, weights, agents_MMS, current_state);
            if (upper_bound <= value_of_best_solution)
                continue; // If the upper bound is less than our best solution thus
                          // far we infer that it can't become better

            // --- CHECK BOUND AND BOUND ---
            if (CONFIGURATION.options.bound_and_bound)
            {
                if (bound || lower_bound > value_of_best_solution)
                {
                    best_solution_yet = state;
                    value_of_best_solution = lower_bound;
                    state_stack.push(current_state);
                    continue;
                }
            }
        }


        vector<pair<state_t, bool>> new_states {};
        // Loop over for each agent and add to the stack the state in which
        // the given agent gets the new good
        for (int i = 0; i < (int)agents.size(); ++i)
        {
            auto new_state = current_state;
            uint64_t new_good_index = picking_order_goods.at(current_state.get_goods_allocated());
            if (new_state.get_agents_weight(i) + weights.at(new_good_index)
                    <= agents.at(i).capacity)
            {
                new_state.allocate_good_to_agent(new_good_index, i, weights.at(new_good_index));
                new_states.emplace_back(new_state, true);
            }
            else
                new_states.emplace_back(new_state, false);
        }
        
        // Add the case where the good goes to charity as well, this needs to
        // be taken into account, we always push this one first, since we want to explore it last
        auto new_state = current_state;
        uint64_t new_good_index = current_state.get_goods_allocated();
        new_state.allocate_to_charity(new_good_index);
        state_stack.push(new_state);
        
        vector<uint64_t> agent_order = get_agents_order(current_state, agents, weights);
        for (auto agent_idx : agent_order)
            if (new_states.at(agent_idx).second)
                state_stack.push(new_states.at(agent_idx).first);

        
    }

    double n = (double)num_agents + 1;
    double m = (double)num_goods;
    double total_num_nodes = (1 - powf(n, m + 1)) / (1 - n);

    cout << "Percent of nodes visited: " << nodes_visited / total_num_nodes << endl;

    return {best_solution_yet.get_allocation(), value_of_best_solution};
}

int main(int argc, char **argv)
{
    handle_options(argc, argv);

    validate_options();

    cout << "Starting branch and bound for batch: " << data_in_filepath << endl;

    // Read in the file data
    ifstream file;

    file.open(data_in_filepath);
    if (!file.is_open())
    {
        cout << "Could not open file: " << data_in_filepath << endl;
        return EXIT_FAILURE;
    }

    string temp {};
    vector<agent_t> agents {};
    vector<weight_t> weights {};
    bool is_first_line {true};
    while (!file.eof())
    {
        getline(file, temp);
        if (temp.empty())
            continue;
        if (is_first_line)
        {
            weights = parse_line_to_weights(temp);
            is_first_line = false;
            continue;
        }
        agents.emplace_back(parse_line_to_values(temp));
    }
    
    file.close();
    cout << "Data is parsed" << endl;

    if (weights.size() != agents.at(0).goods.size())
    {
        cout << "Number of weights and goods does not match" << endl;
        exit(EXIT_FAILURE);
    }

    vector<uint64_t> times {};

    if (RUN_SINGLE_X)
        x_axis_lower_bound = x_axis_upper_bound = agents.at(0).goods.size();
    else if (x_axis_upper_bound > agents.at(0).goods.size())
    {
        cout << "Correcting UPPER BOUND from: " << x_axis_upper_bound << " to: ";
        x_axis_upper_bound =  agents.at(0).goods.size(); 
        cout << x_axis_upper_bound << endl;

        if (x_axis_upper_bound > x_axis_lower_bound)
            x_axis_lower_bound = x_axis_upper_bound;
    }
    for (int run = x_axis_lower_bound; run <= (int)x_axis_upper_bound; ++run)
    {
        vector<agent_t> run_agents = agents;
        for (auto &agent : run_agents)
            agent.goods = {agent.goods.begin(), agent.goods.begin() + run};
        vector<weight_t> run_weights = {weights.begin(), weights.begin() + run};

        int num_agents = agents.size();
        allocation_t allocation(num_agents);
        double alpha_MMS {};
        auto start_time = chrono::high_resolution_clock::now();
       
        // --- PREPROCESS THE ATTRIBUTES FOR THE PROBLEM INSTANCE ----
        // We run this preprocess outside of the BBCMMS due to how we handle
        // the zero value of alpha-MMS values by reducing the instance, that
        // would give us the incorrect attributes for the instance
        find_attributes(agents, weights);
       
        
        if (CONFIGURATION.options.skip)
        {}
        else if (CONFIGURATION.options.mip_solver_active)
            tie(allocation, alpha_MMS) = solve_MIP(run_agents, run_weights);
        else
            tie(allocation, alpha_MMS) = BBCMMS(run_agents, run_weights);
        
        auto end_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

        for (int i = 0; i < num_agents; ++i)
        {
            cout << "Agent " << i + 1 << " got goods: ";
            for (auto good_idx : allocation.get_goods_allocated_to_agent(i))
                cout << good_idx + 1 << " ";
            uint64_t sum {};
            for (auto good_idx : allocation.get_goods_allocated_to_agent(i))
                sum += agents.at(i).goods.at(good_idx).value;
            cout << "with sum value: " << sum <<endl;
        }

        cout << "The alpha-MMS value of the allocation is: " << alpha_MMS << endl;
        cout << "Time: " << duration.count() / 1000 << " ms" << endl;

        times.emplace_back(duration.count());
        CONFIGURATION.time = duration.count();
    }

    
    write_data_to_file(times);


    return EXIT_SUCCESS;
}
