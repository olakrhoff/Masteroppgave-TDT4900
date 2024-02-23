#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <getopt.h>

#include "types.h"

using namespace std;

int NUMBER_OF_AGENTS {};
int NUMBER_OF_GOODS {};
string FILE_OUTPUT_PATH {};

void handle_options(int argc, char **argv)
{
    int code {};
    while ((code = getopt(argc, argv, "a:g:f:")) != -1)
    {
        switch (code)
        {
            case 'f':
                {
                    string temp = optarg;
                    FILE_OUTPUT_PATH = temp;
                    break;
                }
            case 'a':
                {
                    string temp = optarg;
                    NUMBER_OF_AGENTS = stoi(temp);
                    break;
                }
            case 'g':
                {
                    string temp = optarg;
                    NUMBER_OF_GOODS = stoi(temp);
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
    if (NUMBER_OF_AGENTS == 0)
    {
        cout << "Number of agents must be spescified and >0: -a <number>" << endl;
        exit(EXIT_FAILURE);
    }

    if (NUMBER_OF_GOODS == 0)
    {
        cout << "Number of goods must be spescified and >0: -g <number>" << endl;
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
    for (int goods = 0; goods < NUMBER_OF_GOODS; ++goods)
    {
        temp += to_string(get_weight());
        if (goods + 1 < NUMBER_OF_GOODS)
            temp += " ";
    }
    data.emplace_back(temp);

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
