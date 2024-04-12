#include <cstdlib>
#include <numeric>
#include <random>
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <getopt.h>
#include <filesystem>

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

/*
 * This function is a helper function to parse command line arguments on the
 * form "<int>:<int>" to "pair<int, int>".
 */
pair<int, int> get_interval(const string &line)
{
    vector<string> numbers = split(line, ":");

    if (numbers.size() != 2)
    {
        cout << "Should get two numbers back when parsing an interval" << endl;
        exit(EXIT_FAILURE);
    }

    return {stoi(numbers.at(0)), stoi(numbers.at(1))};
}

vector<pair<char, char>> parse_pairwise_string(const string &line)
{
    if (line.size() % 2 != 0)
    {
        cout << "Invalid parsing, must have even number of chars to parse pairwise" << endl;
        exit(EXIT_FAILURE);
    }

    vector<pair<char, char>> list {};

    for (int i = 0; i < (int)line.length() - 1; i += 2)
        list.emplace_back(line.at(i), line.at(i + 1));

    return list;
}

typedef enum ATTRIBUTES
{
    NONE,
    AGENTS,
    GOODS,
    AVG_PERMUTATION_DISTANCE,
    AVG_VALUE_DISTANCE,
    M_OVER_N,
    BUDGET_USED_PERCENT,
} ATTRIBUTES_T;

typedef enum DISTRIBUTIONS
{
    RANDOM,
    NORMAL,
    UNIFORM
} DISTRIBUTIONS_T;


int NUMBER_OF_AGENTS_LOW {};
int NUMBER_OF_AGENTS_HIGH {};
bool NUMBER_OF_AGENTS_ACTIVE {false};
int NUMBER_OF_GOODS_LOW {};
int NUMBER_OF_GOODS_HIGH {};
bool NUMBER_OF_GOODS_ACTIVE {false};
int PERMUTATION_DISTANCE_LOW {};
int PERMUTATION_DISTANCE_HIGH {};
bool PERMUTATION_DISTANCE_ACTIVE {false};
int VALUE_DISTANCE_LOW {};
int VALUE_DISTANCE_HIGH {};
bool VALUE_DISTANCE_ACTIVE {false};
double M_OVER_N_RATIO_LOW {};
double M_OVER_N_RATIO_HIGH {};
bool M_OVER_N_RATIO_ACTIVE {false};
double BUDGET_USED_PERCENT_LOW {1};
double BUDGET_USED_PERCENT_HIGH {1.5};
bool BUDGET_USED_PERCENT_ACTIVE {false};
DISTRIBUTIONS_T BUDGET_DISTRIBUTION {};
DISTRIBUTIONS_T VALUE_DISTRIBUTION {};
bool VALUE_DISTRIBUTION_ACTIVE {false};
DISTRIBUTIONS_T WEIGHT_DISTRIBUTION {};
bool WEIGHT_DISTRIBUTION_ACTIVE {false};

int INTERVALS {};
string FILE_OUTPUT_PATH {};
ATTRIBUTES_T interval_option {NONE};

void handle_options(int argc, char **argv)
{
    int code {};
    while ((code = getopt(argc, argv, "a:g:o:i:p:v:r:b:d:")) != -1)
    {
        switch (code)
        {
            case 'd':
                {
                    string temp = optarg;
                    vector<pair<char, char>> pairs = parse_pairwise_string(temp);

                    for (auto [option, dist] : pairs)
                    {
                        DISTRIBUTIONS_T distribution {};
                        switch (dist)
                        {
                            case 'r':
                                distribution = RANDOM;
                                break;
                            case 'n':
                                distribution = NORMAL;
                                break;
                            case 'u':
                                distribution = UNIFORM;
                                break;
                            default:
                                cout << "Invalid distribution argument (" << dist << ")" << endl;
                                exit(EXIT_FAILURE);
                        }
                        
                        switch (option)
                        {
                            case 'b':
                                BUDGET_DISTRIBUTION = distribution;
                                break;
                            case 'v':
                                VALUE_DISTRIBUTION = distribution;
                                VALUE_DISTRIBUTION_ACTIVE = true;
                                break;
                            case 'w':
                                WEIGHT_DISTRIBUTION = distribution;
                                WEIGHT_DISTRIBUTION_ACTIVE = true;
                                break;
                            default:
                                cout << "Invalid option argument (" << option << ")" << endl;
                                exit(EXIT_FAILURE);
                        }
                    }
                    break;
                }
            case 'b':
                {
                    string temp = optarg;
                    tie(BUDGET_USED_PERCENT_LOW, BUDGET_USED_PERCENT_HIGH) = get_interval(temp);
                    BUDGET_USED_PERCENT_LOW /= 100;
                    BUDGET_USED_PERCENT_HIGH /= 100;
                    BUDGET_USED_PERCENT_ACTIVE = true;
                    break;
                }
            case 'r':
                {
                    string temp = optarg;
                    tie(M_OVER_N_RATIO_LOW, M_OVER_N_RATIO_HIGH) = get_interval(temp);
                    M_OVER_N_RATIO_ACTIVE = true;
                    break;
                }
            case 'v':
                {
                    string temp = optarg;
                    tie(VALUE_DISTANCE_LOW, VALUE_DISTANCE_HIGH) = get_interval(temp);
                    VALUE_DISTANCE_LOW /= 100;
                    VALUE_DISTANCE_HIGH /= 100;
                    VALUE_DISTANCE_ACTIVE = true;
                    break;
                }
            case 'p':
                {
                    string temp = optarg;
                    tie(PERMUTATION_DISTANCE_LOW, PERMUTATION_DISTANCE_HIGH) = get_interval(temp);
                    PERMUTATION_DISTANCE_LOW /= 100;
                    PERMUTATION_DISTANCE_HIGH /= 100;
                    PERMUTATION_DISTANCE_ACTIVE = true;
                    break;
                }
            case 'i':
                {
                    char interval = *optarg;
                       
                    switch (interval)
                    {
                        case 'a':
                            interval_option = AGENTS;
                            break;
                        case 'g':
                            interval_option = GOODS;
                            break;
                        case 'p':
                            interval_option = AVG_PERMUTATION_DISTANCE;
                            break;
                        case 'v':
                            interval_option = AVG_VALUE_DISTANCE;
                            break;
                        case 'r': // Ratio
                            interval_option = M_OVER_N;
                            break;
                        case 'b':
                            interval_option = BUDGET_USED_PERCENT;
                            break;
                        default:
                            cout << "Interval value not recognised" << endl;
                            exit(EXIT_FAILURE);
                    }
                    
                    string temp = (optarg + 1); // We skip the char at the start
                    INTERVALS = stoi(temp);
                    
                    break;
                }
            case 'o':
                {
                    string temp = optarg;
                    FILE_OUTPUT_PATH = temp;
                    break;
                }
            case 'a':
                {
                    string temp = optarg;
                    tie(NUMBER_OF_AGENTS_LOW, NUMBER_OF_AGENTS_HIGH) = get_interval(temp);
                    NUMBER_OF_AGENTS_ACTIVE = true;
                    break;
                }
            case 'g':
                {
                    string temp = optarg;
                    tie(NUMBER_OF_GOODS_LOW, NUMBER_OF_GOODS_HIGH) = get_interval(temp);
                    NUMBER_OF_GOODS_ACTIVE = true;
                    break;
                }
            default:
                cout << "Parsing error for the options: " << (char)optopt << endl;
                exit(EXIT_FAILURE);
        }
    }
}

void validate_options()
{
    if (!NUMBER_OF_AGENTS_ACTIVE)
    {
        cout << "Number of agents must be spescified and >0: -a <number>:<number>" << endl;
        exit(EXIT_FAILURE);
    }

    if (NUMBER_OF_AGENTS_HIGH < NUMBER_OF_AGENTS_LOW)
    {
        cout << "Invalid range for values of agents, start must be lower than (or equal) to the end" << endl;
        exit(EXIT_FAILURE);
    }

    if (!NUMBER_OF_GOODS_ACTIVE && !M_OVER_N_RATIO_ACTIVE)
    {
        cout << "Number of goods must be spescified and >0: -g <number>:<number>" << endl;
        exit(EXIT_FAILURE);
    }

    if (M_OVER_N_RATIO_ACTIVE && NUMBER_OF_GOODS_ACTIVE)
    {
        cout << "Number of goods is given as an argument, but will be overridden by the ratio" << endl;
    }

    if (NUMBER_OF_GOODS_HIGH < NUMBER_OF_GOODS_LOW)
    {
        cout << "Invalid range for values of goods, start must be lower than (or equal) to the end" << endl;
        exit(EXIT_FAILURE);
    }

    if (FILE_OUTPUT_PATH == "" || FILE_OUTPUT_PATH.empty())
    {
        cout << "The output file path must be given: -p <filepath>" << endl;
        exit(EXIT_FAILURE);
    }

    if (PERMUTATION_DISTANCE_ACTIVE && VALUE_DISTANCE_ACTIVE)
    {
        cout << "The avg. permutation distance and the avg. value distance can not be set active at the same time" << endl;
        exit(EXIT_FAILURE);
    }

    if (VALUE_DISTANCE_ACTIVE && VALUE_DISTRIBUTION_ACTIVE)
    {
        cout << "You can not specify a value distribution when avg. value distance is active. The distribution will be ignored" << endl;
        VALUE_DISTRIBUTION_ACTIVE = false;
    }

    if (PERMUTATION_DISTANCE_ACTIVE && 
            (PERMUTATION_DISTANCE_LOW < 0 || PERMUTATION_DISTANCE_LOW > 1 || 
             PERMUTATION_DISTANCE_HIGH < 0 || PERMUTATION_DISTANCE_HIGH > 1))
    {
        cout << "The range for the avg. permuation distance in an interval must be [0, 1]" << endl;
        exit(EXIT_FAILURE);
    }

    if (VALUE_DISTANCE_ACTIVE && 
            (VALUE_DISTANCE_LOW < 0 || VALUE_DISTANCE_LOW > 1 || 
             VALUE_DISTANCE_HIGH < 0 || VALUE_DISTANCE_HIGH > 1))
    {
        cout << "The range for the avg. value distance in an interval must be [0, 1]" << endl;
        exit(EXIT_FAILURE);
    }
}


/*
 * This function takes a value from a normal dist. and maps it to a value in
 * a given interval
 */
double map_normal_dist_to_interval(double lower_bound, double upper_bound, double value)
{
    // We cap the value to 3 standard diviations, this covers 99.7% of the interval and is deemd sufficent 
    if (value < -3.0f) 
        value = -3.0f;
    else if (value > 3.0f)
        value = 3.0f;


    double percent = (value + 3.0f) / 6.0f;

    return percent * (upper_bound - lower_bound);
}


double generate_number(double lower_bound, double upper_bound, DISTRIBUTIONS_T distribution)
{
    random_device rd;
    mt19937 generator(rd());

    student_t_distribution<> normal_dist(MAXFLOAT); // As the degrees of freedom approaches infinity we get the standard normal dist.
    uniform_real_distribution<> uniform_dist(lower_bound, upper_bound);

    switch (distribution)
    {
        case RANDOM:
            if (uniform_dist(generator) > ((upper_bound - lower_bound) / 2))
                return map_normal_dist_to_interval(lower_bound, upper_bound, normal_dist(generator));
            else
                return uniform_dist(generator);
            break;
        case NORMAL:
            return map_normal_dist_to_interval(lower_bound, upper_bound, normal_dist(generator));
            break;
        case UNIFORM:
            return uniform_dist(generator);
            break;
        default:
            cout << "Could not recognise DISTRIBUTION type value (" << (int)WEIGHT_DISTRIBUTION << ")" << endl;
            exit(EXIT_FAILURE);
    }
}

#define MIN_INTERVAL 1
#define MAX_INTERVAL 100

/*
 * This functions generates a weight acording to the options given. Mainly it
 * picks a number in in an interval based on some distribution.
 */
weight_t get_weight()
{
    return generate_number(MIN_INTERVAL, MAX_INTERVAL, WEIGHT_DISTRIBUTION);
}

weight_t get_capacity()
{
    return generate_number(MIN_INTERVAL, MAX_INTERVAL, BUDGET_DISTRIBUTION);   
}

uint64_t get_value_for_good()
{
    return generate_number(MIN_INTERVAL, MAX_INTERVAL, VALUE_DISTRIBUTION);
}

/*
 *  This function is used to generate a random value in an interval drawn from
 *  a uniform distribution.
 */
double get_random_number_from_interval(double lower_bound, double upper_bound)
{
    random_device rd;
    mt19937 generator(rd());

    uniform_int_distribution<> distribution(lower_bound * 100, upper_bound * 100);

    return (double)distribution(generator) / 100;
}

/**
 * This funciton will generate a permutation from the Mallows model.
 * The exact method used is the RIM sampling method.
 */
vector<uint64_t> generate_permutation(double phi, uint64_t num)
{
    if (phi < 0 || phi > 1)
    {
        cout << "The phi value passed to 'generate_permutation' must be [0, 1]" << endl;
        exit(EXIT_FAILURE);
    }

    vector<uint64_t> permutation {};
    
    
    list<uint64_t> temp_permutation {};
    
    // First index will always start here
    temp_permutation.push_front(0);

    // We need to place all the indices
    for (int i = 1; i < (int)num; ++i)
    {
        // We begin at the back of the partial permuation
        int j = i;
        // Loop towards the start
        for (; j > 0; --j)
        {
            // Find the probablity at each step for the chance that we stop here
            // and 'j' bacomes the 'i'-th index's index
            double probablity = pow(phi, i - j); 
            double divider = 0;
            for (int s = 0; s <= i; ++s)
                divider += pow(phi, s);

            probablity /= divider;

            // If probablity holds, then we say that we place current index at this place
            if ((double)(get_random_number_from_interval(0, 100) / 100.0) <= probablity)
                break;
        }

        // Here we actually place the index into its place
        auto itr = temp_permutation.begin();
        for (int s = 0; s < j; ++s)
            itr++;
        
        temp_permutation.insert(itr--, i);
    }

    // Convert list to vector and return the permutation
    for (auto val : temp_permutation)
        permutation.emplace_back(val);

    return permutation;
}

/**
 * This function creates a random normalsied point in the positive part of R^m.
 */
vector<double> generate_random_point(const int m)
{
    vector<double> point;

    for (int i = 0; i < m; ++i)
    {
        double val = (double)get_random_number_from_interval(0, 100);
        point.emplace_back(val / 100);
    }

    return point;
}

vector<double> generate_value_point(const vector<double> &centre, const double gamma)
{
    if (gamma < 0 || gamma > 1)
    {
        cout << "Gamma needs to be in [0,1]" << endl;
        exit(EXIT_FAILURE);
    }

    vector<double> point;

    while (point.size() != centre.size())
    {
        double offset = ((double)get_random_number_from_interval(-100, 100)) / 100;

        offset *= gamma;

        if (centre.at(point.size()) + offset < 0)
            continue;

        point.emplace_back(centre.at(point.size()) + offset);
    }

    return point;
}

typedef struct dataset
{
    int num_agents {};
    int num_goods {};
    vector<int> budgets {};
    vector<int> weights {};
    vector<vector<int>> value_functions {};
} dataset_t;

dataset_t generate_data(const int number_of_goods,
                        const int number_of_agents,
                        const double avg_permutation_distance,
                        const double avg_value_distance,
                        const double m_over_n,
                        const double budget_used_percent)
{
    dataset_t data {};

    data.num_agents = number_of_agents;
    data.num_goods = number_of_goods;

    // If we have M/N ratio active, we need to override the given number of goods
    if (M_OVER_N_RATIO_ACTIVE)
        data.num_goods = data.num_agents * m_over_n;

    // Generate the weights for the goods
    for (int goods = 0; goods < data.num_goods; ++goods)
        data.weights.emplace_back(get_weight());

    // Generate the capacity for the agent and values for the goods
    for (int agents = 0; agents < data.num_agents; ++agents)
        data.budgets.emplace_back(get_capacity());

    // We have now generated the distribution of the weights, we now need to
    // scale it so it fits the budget used percent
    double total_weight = accumulate(data.weights.begin(), data.weights.end(), 0);

    double wanted_total_budget = total_weight / budget_used_percent;
    
    double dist_total_budget = accumulate(data.budgets.begin(), data.budgets.end(), 0);
    
    double scale_factor = wanted_total_budget / dist_total_budget;
    
    transform(data.budgets.begin(), data.budgets.end(), data.budgets.begin(), [scale_factor](double a){ return round(a * scale_factor); });


    // if we will generate avg. value distance
    if (VALUE_DISTANCE_ACTIVE)
    {
        vector<double> centre = generate_random_point(data.num_goods);

        auto double_to_int = [](vector<double> &vec)
        {
            vector<int> res;
            for (auto val : vec)
                res.emplace_back(val * 100);
            return res;
        };

        data.value_functions.emplace_back(double_to_int(centre));

        for (int num_agents = 1; num_agents < data.num_agents; ++num_agents)
        {
            vector<double> point = generate_value_point(centre, avg_value_distance);
            data.value_functions.emplace_back(double_to_int(point));
        }
    }
    else
    {
        // We generate the value functions
        for (int agents = 0; agents < data.num_agents; ++agents)
        {
            vector<int> value_function {};
            for (int goods = 0; goods < data.num_goods; ++goods)
                value_function.emplace_back(get_value_for_good());
            data.value_functions.emplace_back(value_function);
        }
    }

    // If we need to fix the avg. permutation distance
    if (PERMUTATION_DISTANCE_ACTIVE)
    {
        for (int agent_idx = 0; agent_idx < data.num_agents; ++agent_idx)
        {
            auto value_function = data.value_functions.at(agent_idx);
            // Firstly we sort the current values
            sort(value_function.begin(), value_function.end());
            
            // We then generate a permutation
            auto permutation = generate_permutation(avg_permutation_distance, data.num_goods);

            // We then place the sorted values at the indices from the permutation
            for (int i = 0; i < (int)permutation.size(); ++i)
                data.value_functions.at(agent_idx).at(i) = value_function.at(permutation.at(i));
        }
    }

    return data;
}

string get_path_to_folder(const string &filepath)
{
    if (filepath.find('/') == filepath.npos)
        return "."; // We are in the directory

    int index = filepath.find_last_of('/');

    return filepath.substr(0, index);
}

void create_folder_recursive(const string &folder_path)
{
    if (filesystem::exists(folder_path))
        return;

    if (folder_path.find('/') != folder_path.npos)
    {
        create_folder_recursive(folder_path.substr(0, folder_path.find_last_of('/')));
    }
    filesystem::create_directory(folder_path);
}

void write_data_to_file(const dataset_t &data)
{
    // Validate that the folder exists, if not we create it
    string folder_path = get_path_to_folder(FILE_OUTPUT_PATH);

    if (!filesystem::exists(folder_path) || !filesystem::is_directory(folder_path))
    {
        cout << "Could not find folder, creating path now..." << endl;
        try
        {
            create_folder_recursive(folder_path);
        }
        catch (exception e)
        {
            cout << "Could not create folder path: " << folder_path << " " << e.what() << endl;
            exit(EXIT_FAILURE);
        }
        catch (...)
        {
            cout << "Could not create folder path: " << folder_path << endl;
            exit(EXIT_FAILURE);
        }
    }

    if (filesystem::exists(FILE_OUTPUT_PATH))
    {
        cout << "File [" << FILE_OUTPUT_PATH << "] already exists, do you want to override it? (y/n)" << endl;
        string temp;
        do
        {
            getline(cin, temp);
            if (temp == "y")
            {
                break;
            }
            else if (temp == "n")
            {
                cout << "Aborting file creation" << endl;
                exit(EXIT_SUCCESS);
            }
            else
            {
                cout << "Type 'y' for yes or 'n' for no" << endl;
            }
        }
        while (true);
    }

    ofstream file(FILE_OUTPUT_PATH);

    if (!file.is_open())
    {
        cout << "Could not create file: " << FILE_OUTPUT_PATH << endl;
        exit(EXIT_FAILURE);
    }

    // Handle the format of the dataset

    // Firstly, we write all the weights
    for (int i = 0; i < data.num_goods; ++i)
    {
        file << to_string(data.weights.at(i));
        if (i < data.num_goods - 1)
            file << " ";
    }
    file << endl;

    // Then we write all the value functions, prefixed with the agent's budget
    for (int i = 0; i < data.num_agents; ++i)
    {
        file << to_string(data.budgets.at(i)) << ": ";
        for (int j = 0; j < data.num_goods; ++j)
        {
            file << to_string(data.value_functions.at(i).at(j));
            if (j < data.num_goods - 1)
                file << " ";
        }
        file << endl;
    }

    file.close();
}


/*
 * This function gets values in an interval split into equal steps. Such that at
 * step 0 we get the lower bound and at step 'number_of_iterations' we get
 * the upper bound
 */
double get_value_in_interval(double lower_bound, double upper_bound, int iteration, int number_of_iterations)
{
    if (number_of_iterations < 2)
    {
        cout << "Expect at least two iterations in an interval" << endl;
        exit(EXIT_FAILURE);
    }
    double diff = upper_bound - lower_bound;

    double val_per_itr = diff / (number_of_iterations - 1);

    return lower_bound + (val_per_itr * iteration);
}
void add_count_to_filename(string &filename, int count)
{
    auto index = filename.find('.'); // Find where the file extention starts
    if (index == filename.npos)
    {
        cout << "Filename does not have an extention" << endl;
        exit(EXIT_FAILURE);
    }

    string start = filename.substr(0, index);
    string end = filename.substr(index);
    filename = start + to_string(count) + end;
}

int main(int argc, char **argv)
{
    handle_options(argc, argv);
    validate_options();
    
    cout << "Generating data..." << endl;
    if (interval_option == NONE)
    {
        int number_of_goods = get_random_number_from_interval(NUMBER_OF_GOODS_LOW, NUMBER_OF_GOODS_HIGH);
        int number_of_agents = get_random_number_from_interval(NUMBER_OF_AGENTS_LOW, NUMBER_OF_AGENTS_HIGH);
        double avg_permutation_distance = get_random_number_from_interval(PERMUTATION_DISTANCE_LOW, PERMUTATION_DISTANCE_HIGH);
        double avg_value_distance = get_random_number_from_interval(VALUE_DISTANCE_LOW, VALUE_DISTANCE_HIGH);
        double m_over_n = get_random_number_from_interval(M_OVER_N_RATIO_LOW, M_OVER_N_RATIO_HIGH);
        double budget_used_percent = get_random_number_from_interval(BUDGET_USED_PERCENT_LOW, BUDGET_USED_PERCENT_HIGH);

        dataset_t data = generate_data(number_of_goods,
                                       number_of_agents,
                                       avg_permutation_distance,
                                       avg_value_distance,
                                       m_over_n,
                                       budget_used_percent);
    
        write_data_to_file(data);
    }
    else
    {
        for (int i = 0; i < INTERVALS; ++i)
        {
            string temp_name = FILE_OUTPUT_PATH;
            add_count_to_filename(FILE_OUTPUT_PATH, i);
            int number_of_goods = get_random_number_from_interval(NUMBER_OF_GOODS_LOW, NUMBER_OF_GOODS_HIGH);
            int number_of_agents = get_random_number_from_interval(NUMBER_OF_AGENTS_LOW, NUMBER_OF_AGENTS_HIGH);
            double avg_permutation_distance = get_random_number_from_interval(PERMUTATION_DISTANCE_LOW, PERMUTATION_DISTANCE_HIGH);
            double avg_value_distance = get_random_number_from_interval(VALUE_DISTANCE_LOW, VALUE_DISTANCE_HIGH);
            double m_over_n = get_random_number_from_interval(M_OVER_N_RATIO_LOW, M_OVER_N_RATIO_HIGH);
            double budget_used_percent = get_random_number_from_interval(BUDGET_USED_PERCENT_LOW, BUDGET_USED_PERCENT_HIGH);

            switch (interval_option)
            {
                case NONE:
                    cout << "Control flow should not take us here" << endl;
                    exit(EXIT_FAILURE);
                case AGENTS:
                    if (!NUMBER_OF_AGENTS_ACTIVE)
                    {
                        cout << "Number of agents must be specified for it to be used in interval" << endl;
                        exit(EXIT_FAILURE);
                    }
                    number_of_agents = (int)get_value_in_interval(NUMBER_OF_AGENTS_LOW, NUMBER_OF_AGENTS_HIGH, i, INTERVALS);
					break;
                case GOODS:
                    if (!NUMBER_OF_GOODS_ACTIVE)
                    {
                        cout << "Number of goods must be specified for it to be used in interval" << endl;
                        exit(EXIT_FAILURE);
                    }
                    number_of_goods = (int)get_value_in_interval(NUMBER_OF_GOODS_LOW, NUMBER_OF_GOODS_HIGH, i, INTERVALS);
					break;
                case AVG_PERMUTATION_DISTANCE:
                    if (!PERMUTATION_DISTANCE_ACTIVE)
                    {
                        cout << "Permutation distance must be specified for it to be used in interval" << endl;
                        exit(EXIT_FAILURE);
                    }
                    avg_permutation_distance = get_value_in_interval(PERMUTATION_DISTANCE_LOW, PERMUTATION_DISTANCE_HIGH, i, INTERVALS);
					break;
                case AVG_VALUE_DISTANCE:
                    if (!VALUE_DISTANCE_ACTIVE)
                    {
                        cout << "Value distance must be specified for it to be used in interval" << endl;
                        exit(EXIT_FAILURE);
                    }
                    avg_value_distance = get_value_in_interval(VALUE_DISTANCE_LOW, VALUE_DISTANCE_HIGH, i, INTERVALS);
					break;
                case M_OVER_N:
                    if (!M_OVER_N_RATIO_ACTIVE)
                    {
                        cout << "M / N ratio must be specified for it to be used in interval" << endl;
                        exit(EXIT_FAILURE);
                    }
                    m_over_n = get_value_in_interval(M_OVER_N_RATIO_LOW, M_OVER_N_RATIO_HIGH, i, INTERVALS);
					break;
                case BUDGET_USED_PERCENT:
                    if (!BUDGET_USED_PERCENT_ACTIVE)
                    {
                        cout << "Budget percent must be specified for it to be used in interval" << endl;
                        exit(EXIT_FAILURE);
                    }
                    budget_used_percent = get_value_in_interval(BUDGET_USED_PERCENT_LOW, BUDGET_USED_PERCENT_HIGH, i, INTERVALS);
					break;
                default:
                    cout << "Could not recognise interval option" << endl;
                    exit(EXIT_FAILURE);
            }

            dataset_t data = generate_data(number_of_goods,
                                           number_of_agents,
                                           avg_permutation_distance,
                                           avg_value_distance,
                                           m_over_n,
                                           budget_used_percent);
            write_data_to_file(data);
            FILE_OUTPUT_PATH = temp_name;
        }
    }
    
    cout << "Data generated" << endl;

    return EXIT_SUCCESS;
}
