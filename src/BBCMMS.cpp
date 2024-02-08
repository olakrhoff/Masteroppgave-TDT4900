#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>
#include <stack>
#include <float.h>
#include <getopt.h>

#include "LPSolver.h"

using namespace std;

typedef uint64_t weight_t;


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

string data_in_filepath;
string data_out_filepath;
OPTION_X_T x_axis_option {GOODS};
uint64_t x_axis_lower_bound {}, x_axis_upper_bound {1}; 
OPTION_Y_T y_axis_option {TIME};
#define UPPER_BOUND_MIN_LIMIT 7

typedef struct OPTIONS
{
    bool UPPER_BOUND {false};
    bool BOUND_AND_BOUND {false};
} OPTIONS_T;

OPTIONS_T OPTIONS {};
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
 * -u (UPPER_BOUND): takes no argument, simply turns on opt. when given
 * -b (BOUND_AND_BOUND): takes no argument, simply turns on opt. when given
 *                       this will turn on the UPPER_BOUND as well, since it
 *                       is required
 */
void handle_options(int argc, char **argv)
{
    int code {};
    while ((code = getopt(argc, argv, "d:o:x:y:ub")) != -1)
    {
        switch (code)
        {
            case 'b':
                OPTIONS.UPPER_BOUND = true;
                OPTIONS.BOUND_AND_BOUND = true;
                break;
            case 'u':
                OPTIONS.UPPER_BOUND = true;
                break;
            case 'o':
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
                    auto bounds = split(temp, ":");
                    x_axis_lower_bound = stoi(bounds.at(0));
                    x_axis_upper_bound = stoi(bounds.at(1));
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

    if (OPTIONS.UPPER_BOUND)
        data_out_filepath += "_UPPER_BOUND";

    data_out_filepath += ".txt";

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

typedef struct good
{
    double value {};

    good(double value) : value(value)
    {}
} good_t;

typedef struct agent
{
    vector<good_t> goods {};
    uint64_t capacity {};
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
        const vector<good_t> &valuation)
{
    double value = DBL_MAX;
    for (int i = 0; i < (int)allocation.get_number_of_agents(); ++i)
    {
        double temp {};
        for (auto good : allocation.get_goods_allocated_to_agent(i))
            temp += valuation.at(good).value;
        if (temp < value)
            value = temp;
    }
    return value;
}


tuple<double, state_t, bool> upper_bound(vector<agent_t> agents, vector<weight_t> weights,
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
    double solution {};
    vector<double> variables {};
    tie(solution, variables) = solve_LP_simplex(c, A, b, bounds); 


    // --- HANDLE LOWER BOUND ---
    if (OPTIONS.BOUND_AND_BOUND)
    {
        // The entire premis of the lower bound is to make the upper bound a
        // feasible solution which will then work as our lower bound
        for (int j = 0; j < num_variables / num_agents; ++j)
        {
            int agent_idx {};
            double value {};
            // First identifiy the agent whose gonna get the good
            for (int i = 0; i < num_agents; ++i)
            {
                auto var = variables.at(i * (num_variables / num_agents) + j);
                if (var > value)
                {
                    agent_idx = i;
                    value = var;
                }
            }
            for (int i = 0; i < num_agents; ++i)
            {
                auto &var = variables.at(i * (num_variables / num_agents) + j);
                if (i == agent_idx)
                    var = 1;
                else
                    var = 0;
            }
        }
        
        // Now that we have rounded the LP to an integer answer we must check
        // if it is feasible, if not, we need to reduce the allocations until
        // it is. 
        
        //double lower_bound = min_bundle(possible_state.get_allocation().goods_for_agents, agents);
        // TODO: Fix this after cleanup
        double lower_bound = -1;
        
        // If the lower bound and upper bound are the same, then we have a tight
        // solution and we can say that this will be the answer of the subproblem
        // meaning we can prune the whole search of the tree below it
        if (lower_bound == solution)
        {
            return {solution, current_state, true};
        }
    }
    
    return {solution, current_state, false};
}

tuple<double, state_t, bool> upper_bound_with_MMS(vector<agent_t> agents, const vector<weight_t> &weights,
                            vector<double> MMS, const state_t &current_state)
{
    for (int i = 0; i < (int)agents.size(); ++i)
        for (int j = 0; j < (int)agents.at(i).goods.size(); ++j)
            agents.at(i).goods.at(j).value /= MMS.at(i);

    return upper_bound(agents, weights, current_state);
}

tuple<double, state_t, bool> upper_bound_find_MMS(const agent_t &agent, const vector<weight_t> &weights,
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
    double value_of_best_solution {};
    /*
     
    vector<pair<good_t, weight_t>> goods {};
    for (int i = 0; i < weights.size(); ++i)
        goods.emplace_back(agent.goods.at(i), weights.at(i));

    sort(goods.begin(), goods.end(), [](auto a, auto b){ return a.first.value > b.first.value; });
    auto capacity {agent.capacity};
    for (int i = num_agents - 1; i < goods.size(); i += num_agents)
    {
        auto good {goods.at(i)};
        if (capacity < good.second)
            continue;
        capacity -= good.second;
        value_of_best_solution += good.first.value;
    }
    */

    stack<state_t> state_stack {};
    state_t start_state(num_agents);
    state_stack.push(start_state);

    state_t best_solution_yet = start_state;
    while (!state_stack.empty())
    {
        auto current_state = state_stack.top();
        state_stack.pop();

        double val {};
        state_t state(num_goods * num_agents);
        bool bound {};
        // --- CHECK UPPER BOUND ---
        if (OPTIONS.UPPER_BOUND && num_goods - current_state.get_goods_allocated() >= UPPER_BOUND_MIN_LIMIT)
        {
            tie(val, state, bound) = upper_bound_find_MMS(agent, weights, current_state);
            if (val <= value_of_best_solution)
                continue; // If the upper bound is less than our best solution thus
            if (OPTIONS.BOUND_AND_BOUND && bound)
            {
                best_solution_yet = state;
                value_of_best_solution = val;
            }
        }
                          // far we infer that it can't become better

        // If we have allocated all the goods we now need to evaluate the
        // allocation and score it
        if (current_state.get_goods_allocated() == num_goods)
        {
            double value = min_bundle(current_state.get_allocation(), 
                    agent.goods);
            if (value > value_of_best_solution)
            {
                value_of_best_solution = value;
                best_solution_yet = current_state;
            }
            continue;
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

/**
 * This is the main funtion for the algorithm
 * Here we will run the B&B algorithm to solve the fair allocation of
 * budget symmetry illustion maximin share (BSIMMS)
 */
pair<allocation_t, double> BBCMMS(const vector<agent_t> &agents, 
                                              const vector<weight_t> &weights)
{
    double nodes_visited {};
    uint64_t num_goods = agents.at(0).goods.size();
    uint64_t num_agents = agents.size();

    vector<double> agents_MMS {};
    for (int i = 0; i < (int)num_agents; ++i)
    {
        cout << "Finding MMS for agent " << i + 1 << endl;
        agents_MMS.emplace_back(find_MMS(agents.at(i), num_agents, weights));
        if (agents_MMS.back() == 0)
        {
            auto reduced_agents = agents;
            auto index = agents_MMS.size() - 1;
            reduced_agents.erase(reduced_agents.begin() + index);
            return BBCMMS(reduced_agents, weights);
        }
        cout << "MMS for agent " << i + 1 << " is: " << agents_MMS.back() << endl;
    }
    cout << "MMS found for all agents" << endl;

    // --- HANDLE THE CASE WHERE WE GET A ZERO MMS VALUE ---
    // We simply remove the first one that got a zero value and solve without it


    // --- SETTING THE PICKING ORDER ---
    vector<uint64_t> picking_order_goods {};
    for (int i = 0; i < (int)num_goods; ++i)
        picking_order_goods.push_back(i);

    // Since we don't have any picking order optimisation set yet we do a
    // little random shuffle of the picking order to get some more rounded data
    random_device rd;
    mt19937 g(rd());
    shuffle(picking_order_goods.begin(), picking_order_goods.end(), g);


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
    double value_of_best_solution {0.333};
    while (!state_stack.empty())
    {
        nodes_visited++;

        auto current_state = state_stack.top();
        state_stack.pop();


        // --- CHECK UPPER BOUND ---
        if (OPTIONS.UPPER_BOUND && num_goods - current_state.get_goods_allocated() >= UPPER_BOUND_MIN_LIMIT)
        {
            double val {};
            state_t state(num_goods * num_agents);
            bool bound {};
            tie(val, state, bound) = upper_bound_with_MMS(agents, weights, agents_MMS, current_state);
            if (val <= value_of_best_solution)
                continue; // If the upper bound is less than our best solution thus
                          // far we infer that it can't become better
            if (OPTIONS.BOUND_AND_BOUND && bound)
            {
                best_solution_yet = state;
                value_of_best_solution = val;
            }
        }

        // If we have allocated all the goods we now need to evaluate the
        // allocation and score it
        if (current_state.get_goods_allocated() == num_goods)
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
                state_stack.push(new_state);
            }
        }
        // Add the case where the good goes to charity as well, this needs to
        // be taken into account
        auto new_state = current_state;
        uint64_t new_good_index = current_state.get_goods_allocated();
        new_state.allocate_to_charity(new_good_index);
        state_stack.push(new_state);
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

    if (x_axis_upper_bound > agents.at(0).goods.size())
    {
        cout << "Correcting UPPER BOUND from: " << x_axis_upper_bound << " to: ";
        x_axis_upper_bound =  agents.at(0).goods.size(); 
        cout << x_axis_upper_bound << endl;
    }
    for (int run = x_axis_lower_bound; run <= (int)x_axis_upper_bound; ++run)
    {
        vector<agent_t> run_agents = agents;
        for (auto &a : run_agents)
            a.goods = {a.goods.begin(), a.goods.begin() + run};
        vector<weight_t> run_weights = {weights.begin(), weights.begin() + run};

        allocation_t allocation(run_agents.size());
        double alpha_MMS {};
        auto start_time = chrono::high_resolution_clock::now();
        tie(allocation, alpha_MMS) = BBCMMS(run_agents, run_weights);
        auto end_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

        for (int i = 0; i < (int)run_agents.size(); ++i)
        {
            cout << "Agent " << i + 1 << " got goods: ";
            for (auto good : allocation.get_goods_allocated_to_agent(i))
                cout << good + 1 << " ";
            uint64_t sum {};
            for (auto g : allocation.get_goods_allocated_to_agent(i))
                sum += agents.at(i).goods.at(g).value;
            cout << "with sum value: " << sum <<endl;
        }

        cout << "The alpha-MMS value of the allocation is: " << alpha_MMS << endl;
        cout << "Time: " << duration.count() / 1000 << " ms" << endl;

        times.emplace_back(duration.count());
    }

    
    write_data_to_file(times);


    return EXIT_SUCCESS;
}
