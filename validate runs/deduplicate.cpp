#include <iostream>
#include <fstream>
#include <map>

using namespace std;

vector<string> split(const string &line, const string &delimiter)
{
    vector<string> result;

    string temp = line;
    while ((int)temp.find(delimiter) != -1)
    {
        result.emplace_back(temp.substr(0, temp.find(delimiter)));
        temp = temp.substr(temp.find(delimiter) + delimiter.length());
    }
    result.emplace_back(temp);

    return result;
}

void add_to_map(map<string, int> &removes, string &line)
{
    while (line.at(0) == ' ')
        line = line.substr(1);
    int count = line.at(0) - '0';
    count--;
    string temp = line.substr(2);

    removes.insert(make_pair(temp, count));
}


string get_unique(const string &line)
{
    string temp;

    temp += line.substr(0, line.find(','));

    auto vals = split(line, ",");

    for (int i = 7; i < vals.size() - 1; ++i)
        temp += "," + vals.at(i);

    return temp;
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        cerr << "Usage: " << argv[0] << " <source> <duplicates> <dest>" << endl;
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
        string key = get_unique(line);

        if (removes.find(key) != removes.end())
        {
            if (removes.at(key) > 0)
            {
                removes.at(key)--;
                continue;
            }
        }
        outfile << line << endl;
    }


    return 0;
}
