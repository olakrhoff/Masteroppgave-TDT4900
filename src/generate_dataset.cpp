#include <cstdlib>
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

int NUMBER_OF_AGENTS_LOW {};
int NUMBER_OF_AGENTS_HIGH {};
int NUMBER_OF_GOODS_LOW {};
int NUMBER_OF_GOODS_HIGH {};
string FILE_OUTPUT_PATH {};

void handle_options(int argc, char **argv)
{
    int code {};
    while ((code = getopt(argc, argv, "a:g:o:")) != -1)
    {
        switch (code)
        {
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

vector<string> generate_data()
{
    vector<string> data {};

    string temp {};
    // Generate the weights for the goods
    for (int goods = 0; goods < NUMBER_OF_GOODS; ++goods)
    {
        temp += to_string(get_weight());
        if (goods + 1 < NUMBER_OF_GOODS)
            temp += " ";
    }
    data.emplace_back(temp);

    // Generate the capacity for the agent and values for the goods
    for (int agents = 0; agents < NUMBER_OF_AGENTS; ++agents)
    {
        temp = "";
        temp += to_string(get_capacity()) + ": ";
        
        for (int goods = 0; goods < NUMBER_OF_GOODS; ++goods)
        {
            temp += to_string(get_good());
            if (goods + 1 < NUMBER_OF_GOODS)
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

int main(int argc, char **argv)
{
    handle_options(argc, argv);
    validate_options();
    
    cout << "Generating data..." << endl;
    vector<string> data = generate_data();
    
    write_data_to_file(data);
    
    cout << "Data generated" << endl;

    return EXIT_SUCCESS;
}
