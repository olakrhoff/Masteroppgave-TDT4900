#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

/**
 * This is code that I have reused from my earlier projects, look at my github 
 */
vector<string> split(const string &line, const char delimiter)
{
    vector<string> result {};

    string temp = line;
    while (temp.find(delimiter) != -1)
    {
        result.emplace_back(temp.substr(0, temp.find(delimiter)));
        temp = temp.substr(temp.find(delimiter) + 1);
    }
    result.emplace_back(temp);

    return result;
}

vector<uint64_t> parse_line_to_values(const string &line)
{
    vector<uint64_t> values {};
    
    for (auto string_val : split(line, ' '))
        values.emplace_back(stoi(string_val));
    return values;
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
    vector<vector<uint64_t>> agents {};
    while (!file.eof())
    {
        getline(file, temp);
        if (temp.empty())
            continue;
        agents.emplace_back(parse_line_to_values(temp));
    }
    
    file.close();


    for (int i = 0; i < agents.size(); ++i)
    {
        cout << "Agent " << i + 1 << " has values: ";
        for (auto val : agents.at(i))
            cout << val << " ";
        cout << endl;
    }



    return EXIT_SUCCESS;
}
