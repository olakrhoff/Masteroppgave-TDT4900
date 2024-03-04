#include <cstdlib>
#include <random>
#include <iostream>
#include <fstream>
#include <vector>
#include <getopt.h>

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

typedef enum ATTRIBUTES
{
    NONE,
    AGENTS,
    GOODS,
    AVG_PERMUTATION_DISTANCE,
    AVG_VALUE_DISTANCE,
    M_OVER_N,
    BUDGET_USED_PERCENT
} ATTRIBUTES_T;


int NUMBER_OF_AGENTS_LOW {};
int NUMBER_OF_AGENTS_HIGH {};
int NUMBER_OF_GOODS_LOW {};
int NUMBER_OF_GOODS_HIGH {};
int PERMUTATION_DISTANCE_LOW {};
int PERMUTATION_DISTANCE_HIGH {};
int VALUE_DISTANCE_LOW {};
int VALUE_DISTANCE_HIGH {};
int M_OVER_N_RATIO_LOW {};
int M_OVER_N_RATIO_HIGH {};
int BUDGET_USED_PERCENT_LOW {};
int BUDGET_USED_PERCENT_HIGH {};

int INTERVALS {};
string FILE_OUTPUT_PATH {};
ATTRIBUTES_T interval_option {NONE};

void handle_options(int argc, char **argv)
{
    int code {};
    while ((code = getopt(argc, argv, "a:g:o:i:p:v:r:b:")) != -1)
    {
        switch (code)
        {
            case 'b':
                {
                    string temp = optarg;
                    tie(BUDGET_USED_PERCENT_LOW, BUDGET_USED_PERCENT_HIGH) = get_interval(temp);
                    break;
                }
            case 'r':
                {
                    string temp = optarg;
                    tie(M_OVER_N_RATIO_LOW, M_OVER_N_RATIO_HIGH) = get_interval(temp);
                    break;
                }
            case 'v':
                {
                    string temp = optarg;
                    tie(VALUE_DISTANCE_LOW, VALUE_DISTANCE_HIGH) = get_interval(temp);
                    break;
                }
            case 'p':
                {
                    string temp = optarg;
                    tie(PERMUTATION_DISTANCE_LOW, PERMUTATION_DISTANCE_HIGH) = get_interval(temp);
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
                    break;
                }
            case 'g':
                {
                    string temp = optarg;
                    tie(NUMBER_OF_GOODS_LOW, NUMBER_OF_GOODS_HIGH) = get_interval(temp);
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
    if (NUMBER_OF_AGENTS_LOW == 0 || NUMBER_OF_GOODS_HIGH == 0)
    {
        cout << "Number of agents must be spescified and >0: -a <number>:<number>" << endl;
        exit(EXIT_FAILURE);
    }

    if (NUMBER_OF_AGENTS_HIGH < NUMBER_OF_AGENTS_LOW)
    {
        cout << "Invalid range for values of agents, start must be lower than (or equal) to the end" << endl;
        exit(EXIT_FAILURE);
    }

    if (NUMBER_OF_GOODS_LOW == 0 || NUMBER_OF_GOODS_HIGH == 0)
    {
        cout << "Number of goods must be spescified and >0: -g <number>:<number>" << endl;
        exit(EXIT_FAILURE);
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
}

weight_t get_weight()
{
    return 1;
}

weight_t get_capacity()
{
    return 10;
}

uint64_t get_good()
{
    return 2;
}

vector<string> generate_data(const int number_of_goods, const int number_of_agents)
{
    vector<string> data {};

    string temp {};
    // Generate the weights for the goods
    for (int goods = 0; goods < number_of_goods; ++goods)
    {
        temp += to_string(get_weight());
        if (goods + 1 < number_of_goods)
            temp += " ";
    }
    data.emplace_back(temp);

    // Generate the capacity for the agent and values for the goods
    for (int agents = 0; agents < number_of_agents; ++agents)
    {
        temp = "";
        temp += to_string(get_capacity()) + ": ";
        
        for (int goods = 0; goods < number_of_goods; ++goods)
        {
            temp += to_string(get_good());
            if (goods + 1 < number_of_goods)
                temp += " ";
        }
        data.emplace_back(temp);
    }

    return data;
}

void write_data_to_file(const vector<string> &data)
{
    ofstream file(FILE_OUTPUT_PATH);

    if (!file.is_open())
    {
        cout << "Could not create file: " << FILE_OUTPUT_PATH << endl;
        exit(EXIT_FAILURE);
    }

    for (auto line : data)
        file << line << "\n";

    file.close();
}

/*
 *  This function is used to generate a random value in an interval drawn from
 *  a uniform distribution.
 */
int get_random_number_from_interval(int lower_bound, int upper_bound)
{
    random_device rd;
    mt19937 generator(rd());

    uniform_int_distribution<> distribution(lower_bound, upper_bound);

    return distribution(generator);
}

/*
 * This function gets values in an interval split into equal steps. Such that at
 * step 0 we get the lower bound and at step 'number_of_iterations' we get
 * the upper bound
 */
double get_value_in_interval(int lower_bound, int upper_bound, int iteration, int number_of_iterations)
{
    if (number_of_iterations < 2)
    {
        cout << "Expect at least two iterations in an interval" << endl;
        exit(EXIT_FAILURE);
    }
    double diff = (double)(upper_bound - lower_bound);

    double val_per_itr = diff / (number_of_iterations - 1);

    return lower_bound + (val_per_itr * iteration);
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

        vector<string> data = generate_data(number_of_goods, number_of_agents);
    
        write_data_to_file(data);
    }
    else
    {
        for (int i = 0; i < INTERVALS; ++i)
        {
            string temp_name = FILE_OUTPUT_PATH;
            FILE_OUTPUT_PATH += "_" + to_string(i);
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
                    number_of_agents = get_value_in_interval(NUMBER_OF_AGENTS_LOW, NUMBER_OF_AGENTS_HIGH, i, INTERVALS);
					break;
                case GOODS:
                    number_of_goods = get_value_in_interval(NUMBER_OF_GOODS_LOW, NUMBER_OF_GOODS_HIGH, i, INTERVALS);
					break;
                case AVG_PERMUTATION_DISTANCE:
                    avg_permutation_distance = get_value_in_interval(PERMUTATION_DISTANCE_LOW, PERMUTATION_DISTANCE_HIGH, i, INTERVALS);
					break;
                case AVG_VALUE_DISTANCE:
                    avg_value_distance = get_value_in_interval(VALUE_DISTANCE_LOW, VALUE_DISTANCE_HIGH, i, INTERVALS);
					break;
                case M_OVER_N:
                    m_over_n = get_value_in_interval(M_OVER_N_RATIO_LOW, M_OVER_N_RATIO_HIGH, i, INTERVALS);
					break;
                case BUDGET_USED_PERCENT:
                    budget_used_percent = get_value_in_interval(BUDGET_USED_PERCENT_LOW, BUDGET_USED_PERCENT_HIGH, i, INTERVALS);
					break;
                default:
                    cout << "Could not recognise interval option" << endl;
                    exit(EXIT_FAILURE);
            }

            vector<string> data = generate_data(number_of_goods, number_of_agents);
            write_data_to_file(data);
            FILE_OUTPUT_PATH = temp_name;
        }
    }
    
    cout << "Data generated" << endl;

    return EXIT_SUCCESS;
}
