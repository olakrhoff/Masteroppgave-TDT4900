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

int main()
{
    const string file = "global_results_batch_1.2.csv";
    const string removes_file = "jobs_to_delete_and_reschedule.txt";
    const string out = "global_results_batch_1.3.csv";


    map<string, int> removes;

    ifstream infile(removes_file);
    if (!infile.is_open())
    {
        cout << "FAIL 1" << endl;;
        return 1;
    }
    
    string temp;
    while (getline(infile, temp)) 
        add_to_map(removes, temp);

    infile.close();

    infile.open(file);

    if (!infile.is_open())
    {
        cout << "FAIL 2" << endl;
        return 1;
    }
    ofstream outfile(out);

    if (!outfile.is_open())
    {
        cout << "FAIL 3" << endl;
        return 1;
    }

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
