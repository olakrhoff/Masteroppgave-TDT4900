#include <iostream>
#include <fstream>
#include <map>

using namespace std;

void add_to_map(map<string, int> &removes, string &line)
{
    while (line.at(0) == ' ')
        line = line.substr(1);
    int count = line.at(0) - '0';
    count--;
    string temp = line;

    removes.insert(make_pair(temp, count));
}

int main(int argc, char **argv)
{
    
    if (argc != 4)
    {
        cerr << "Usage: " << argv[0] << " <input_file> <removes_file> <output_file>" << endl;
        exit(1);
    }

    const string file = argv[1];
    const string removes_file = argv[2];
    const string out = argv[3];


    map<string, int> removes;

    ifstream infile(removes_file);
    if (!infile.is_open())
        return 1;
    
    string temp;
    while (getline(infile, temp)) 
        add_to_map(removes, temp);

    infile.close();

    infile.open(file);

    if (!infile.is_open())
        return 1;
    
    ofstream outfile(out);

    if (!outfile.is_open())
        return 1;

    string line;
    bool is_first_line = true;
    while (getline(infile, line))
    {
        if (is_first_line)
        {
            is_first_line = false;
            outfile << line << endl;
            continue;
        }
        string key = line.substr(0, line.find(',')); 

        if (removes.find(key) != removes.end())
            continue;

        outfile << line << endl;
    }


    return 0;
}
