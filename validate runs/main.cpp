#include <iostream>
#include <fstream>

using namespace std;

double get_number(const string &line, int index)
{
    for (int i = index; i < line.length(); ++i)
    {
        if (line.at(i) == ',')
        {
            string num = line.substr(index, i - index);
            return stod(num);
        }
    }
}

string replace_number(const string &line, int index, const string& new_string)
{
    string res = line.substr(0, index) + new_string;
    for (int i = index; i < line.length(); ++i)
    {
        if (line.at(i) == ',')
        {
            res += line.substr(i);
            break;
        }
    }

    return res;
}

string edit_line(string &line)
{
    int commas = 0;
    double avg_perm = 0;
    double m = 0;
    for (int i = 0; i < line.length(); ++i)
    {
        if (line.at(i) == ',')
        {
            commas++;
            i+=2;
            if (commas == 2)
            {
                m = get_number(line, i);
            }
            if (commas == 4)
            {
                avg_perm = get_number(line, i);
       
                if (avg_perm >= 0 && avg_perm <= 1)
                    return line;

                double max_perm = 0.5 * m * (m - 1);
                double new_perm = avg_perm / max_perm;

                string new_line = replace_number(line, i, to_string(new_perm));
                return new_line;
            }
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <source> <dest>" << endl;
        exit(1);
    }

    const string file = argv[1];
    const string out = argv[2];

    ifstream infile(file);

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

        outfile << edit_line(line) << endl;
    }


    return 0;
}
