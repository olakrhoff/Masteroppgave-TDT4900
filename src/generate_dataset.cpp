#include <cstdlib>
#include <iostream>
#include <vector>
#include <getopt.h>

using namespace std;





int NUMBER_OF_AGENTS {};

void handle_options(int argc, char **argv)
{
    int code {};
    while ((code = getopt(argc, argv, "")) != -1)
    {
        switch (code)
        {
            default:
                cout << "Parsing error for the options: " << (char)optopt << endl;
                exit(EXIT_FAILURE);
        }
    }
}



vector<string> generate_data()
{
    vector<string> data {};

    string temp {};
    for (int goods = 0; goods < NUMBER_OF_GOODS; ++goods)
        temp += get_weight() + " ";
    data.emplace_back(temp);

    for (int agents = 0; agents < NUMBER_OF_AGENTS; ++agents)
    {
        
    }

    return data;
}

void write_data_to_file(const vector<string> &data)
{

}

int main(int argc, char **argv)
{
    cout << "Generating data..." << endl;

    handle_options(argc, argv);

    vector<string> data = generate_data();

    write_data_to_file(data);

    return EXIT_SUCCESS;
}
