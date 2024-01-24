#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>
#include <stack>
#include <float.h>

using namespace std;

typedef uint64_t weight_t;

/**
 * This is code that I have reused from my earlier projects, look at my github 
 */
vector<string> split(const string &line, const string &delimiter)
{
    vector<string> result {};

    string temp = line;
    while (temp.find(delimiter) != -1)
    {
        result.emplace_back(temp.substr(0, temp.find(delimiter)));
        temp = temp.substr(temp.find(delimiter) + delimiter.length());
    }
    result.emplace_back(temp);

    return result;
}

typedef struct good
{
    uint64_t value {};

    good(uint64_t value) : value(value)
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
    vector<vector<uint64_t>> goods_for_agents {};
    vector<weight_t> weight {};

    allocation(uint64_t num_agents)
    {
        for (int i = 0; i < num_agents; ++i)
        {
            goods_for_agents.emplace_back(vector<uint64_t>());
            weight.emplace_back(0);
        }
    }
} allocation_t;

typedef struct state
{
    private:
    allocation_t allocation;
    vector<uint64_t> charity {};
    uint64_t goods_allocated {};

    public:
    state(uint64_t num_agents) : allocation(num_agents)
    {}

    void allocate_good_to_agent(uint64_t good_index, uint64_t agent_index, uint64_t goods_weight)
    {
        allocation.goods_for_agents.at(agent_index).emplace_back(good_index);
        goods_allocated++;
        allocation.weight.at(agent_index) += goods_weight;
    }

    void allocate_to_charity(uint64_t good_index)
    {
        charity.emplace_back(good_index);
        goods_allocated++;
    }

    weight_t get_agents_weight(const uint64_t agent_index)
    {
        return allocation.weight.at(agent_index);
    }

    uint64_t get_goods_allocated() const
    {
        return goods_allocated;
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
double alpha_MMS_of_allocation(const vector<vector<uint64_t>> &allocation, 
        const vector<agent_t> &valuation,
        const vector<double> &agent_MMS)
{
    double value = DBL_MAX;
    for (int i = 0; i < allocation.size(); ++i)
    {
        double temp {};
        for (auto good : allocation.at(i))
            temp += valuation.at(i).goods.at(good).value;
        temp /= agent_MMS.at(i);
        if (temp < value)
            value = temp;
    }
    return value;
}

double min_bundle(const vector<vector<uint64_t>> &allocation,
        const vector<good_t> &valuation)
{
    double value = DBL_MAX;
    for (int i = 0; i < allocation.size(); ++i)
    {
        double temp {};
        for (auto good : allocation.at(i))
            temp += valuation.at(good).value;
        if (temp < value)
            value = temp;
    }
    return value;
}

/**
 * This function is finds the MMS for an agent, given it's value function
 * and the number of agents in the mix
 */
double find_MMS(const agent_t &agent, uint64_t num_agents, const vector<weight_t> &weights)
{
    uint64_t num_goods = agent.goods.size();

    stack<state_t> state_stack {};
    state_t start_state(num_agents);
    state_stack.push(start_state);

    state_t best_solution_yet = start_state;
    double value_of_best_solution {};
    while (!state_stack.empty())
    {
        auto current_state = state_stack.top();
        state_stack.pop();

        // If we have allocated all the goods we now need to evaluate the
        // allocation and score it
        if (current_state.get_goods_allocated() == num_goods)
        {
            double value = min_bundle(current_state.get_allocation().goods_for_agents, 
                    agent.goods);
            if (value > value_of_best_solution)
            {
                cout << "Current best solution is: " << value << endl;
                value_of_best_solution = value;
                best_solution_yet = current_state;
            }
            continue;
        }

        // Loop over for each agent and add to the stack the state in which
        // the given agent gets the new good
        for (int i = 0; i < num_agents; ++i)
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
pair<vector<vector<uint64_t>>, double> BBCMMS(const vector<agent_t> &agents, 
                                              const vector<weight_t> &weights)
{
    uint64_t num_goods = agents.at(0).goods.size();
    uint64_t num_agents = agents.size();
    vector<double> agents_MMS {};
    for (int i = 0; i < num_agents; ++i)
    {
        cout << "Finding MMS for agent " << i + 1 << endl;
        agents_MMS.emplace_back(find_MMS(agents.at(i), num_agents, weights));
    }
    cout << "MMS found for all agents" << endl;
    vector<uint64_t> picking_order_goods {};
    for (int i = 0; i < num_goods; ++i)
        picking_order_goods.push_back(i);

    // Since we don't have any picking order optimisation set yet we do a
    // little random shuffle of the picking order to get some more rounded data
    random_device rd;
    mt19937 g(rd());
    shuffle(picking_order_goods.begin(), picking_order_goods.end(), g);

    // We want to traverse the search space in a depth-first manner
    // Therefore we create a stack to keep track of where we are in
    // the current state

    stack<state_t> state_stack {};
    state_t start_state(num_agents);
    state_stack.push(start_state);

    state_t best_solution_yet = start_state;
    double value_of_best_solution {};
    while (!state_stack.empty())
    {
        auto current_state = state_stack.top();
        state_stack.pop();

        // If we have allocated all the goods we now need to evaluate the
        // allocation and score it
        if (current_state.get_goods_allocated() == num_goods)
        {
            double value = alpha_MMS_of_allocation(current_state.get_allocation().goods_for_agents,
                    agents, agents_MMS);
            if (value > value_of_best_solution)
            {
                cout << "Current best solution is: " << value << endl;
                value_of_best_solution = value;
                best_solution_yet = current_state;
            }
            continue;
        }

        // Loop over for each agent and add to the stack the state in which
        // the given agent gets the new good
        for (int i = 0; i < agents.size(); ++i)
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

    return {best_solution_yet.get_allocation().goods_for_agents, value_of_best_solution};
}

int main(int argc, char **args)
{
    if (argc != 2)
    {
        cout << "The program needs an argument, as a filepath to the input data" << endl;
        return EXIT_FAILURE;
    }
    string filename = args[1];
    cout << "Starting branch and bound for batch: " << filename << endl;


    // Read in the file data
    ifstream file;

    file.open(filename);
    if (!file.is_open())
    {
        cout << "Could not open file: " << filename << endl;
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


    vector<vector<uint64_t>> allocation {};
    double alpha_MMS {};
    auto start_time = chrono::high_resolution_clock::now();
    tie(allocation, alpha_MMS) = BBCMMS(agents, weights);
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

    for (int i = 0; i < allocation.size(); ++i)
    {
        cout << "Agent " << i + 1 << " got goods: ";
        for (auto good : allocation.at(i))
            cout << good + 1 << " ";
        uint64_t sum {};
        for (auto g : allocation.at(i))
            sum += agents.at(i).goods.at(g).value;
        cout << "with sum value: " << sum <<endl;
    }

    cout << "The alpha-MMS value of the allocation is: " << alpha_MMS << endl;
    cout << "Time: " << duration.count() / 1000 << " ms" << endl;

    return EXIT_SUCCESS;
}
