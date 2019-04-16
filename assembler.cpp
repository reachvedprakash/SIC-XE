#include <iostream>
#include <stdio.h>
#include <map>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <stack>
#include <set>
#include <cstring>
#include <stdio.h>

using namespace std;

string Prgname;
int straddr, lastaddr;
string Basearg;
int basevalue;
bool base;
map<string, int> format, opcode, symtab;
map<string, pair<int, string> > literaltable;
set<string> littab;
map<char, char> regcode;

class Ins
{
public:
    int address;
    string label;
    string instruction;
    string parameter;
    string opcode;
    int objectCode;
    int e;
    int f;
    int n;
    int i;
    int x;
    int b;
    int p;
};
vector<Ins> Program(100);

string intToString(int n, int x = 6)
{ //function to convert int to string of default lenght 6
    stringstream stream;
    stream << hex << n;
    string result(stream.str());
    for (int y = 0; y < result.length(); y++)
        result[y] = toupper(result[y]);
    while (result.length() < x)
        result = "0" + result;
    return result;
}

int strhextoint(string s)
{
    int x;
    stringstream ss;
    ss << hex << s;
    ss >> x;
    return x;
}

void optab()
{
    string ins;
    int oc, frm;
    string str;
    fstream fp("INS.txt");

    while (getline(fp, str))
    {
        istringstream iss(str);
        iss >> ins;
        iss >> frm;
        iss >> hex >> oc;
        format[ins] = frm;
        opcode[ins] = oc;
    }
}

void firstPass(char *filename)
{
    string str;
    int address = 0;
    fstream fp(filename);
    int i = -1;
    while (getline(fp, str))
    {
        istringstream iss(str);
        vector<string> list;
        int count = 0;
        while (iss)
        {
            string sub;
            iss >> sub;
            list.push_back(sub);
            count++;
        }
        if (list[0][0] == '.')
        {
            continue;
        }
        else if (list[0] == "END")
        {
            if (!littab.empty())
            {
                for (auto x : littab)
                {
                    symtab[x] = address;
                    Program[i].address = address;
                    Program[i].label = x;
                    Program[i].instruction = "LLLL";
                    Program[i].parameter = x;
                    Program[i].objectCode = strhextoint(literaltable[x].second);
                    address += literaltable[x].first;
                    Program[i].f = literaltable[x].first;
                    Program[i].opcode = literaltable[x].second;
                    i++;
                }
                littab.clear();
            }
            lastaddr = address;
            return;
        }
        else if (list[0] == "BASE")
        {
            Basearg = list[1];
            base = true;
            continue;
        }
        else if (i == -1 && list[1] == "START")
        {
            Prgname = list[0];

            straddr = strhextoint(list[2]);

            address = straddr;
            i++;
        }
        else if (list[0] == "LTORG")
        {

            for (auto x : littab)
            {
                symtab[x] = address;
                Program[i].address = address;
                Program[i].label = x;
                Program[i].instruction = "LLLL";
                Program[i].parameter = x;
                Program[i].objectCode = strhextoint(literaltable[x].second);
                address += literaltable[x].first;
                Program[i].f = literaltable[x].first;
                Program[i].opcode = literaltable[x].second;
                i++;
            }
            littab.clear();
        }
        else if (list[1] == "RESW")
        {
            Program[i].address = address;
            Program[i].label = list[0];
            symtab[list[0]] = address;
            Program[i].instruction = "RESW";
            Program[i].parameter = list[2];
            address += stoi(list[2]) * 3;
            Program[i].f = 0;
            i++;
        }
        else if (list[1] == "RESB")
        {
            Program[i].address = address;
            Program[i].label = list[0];
            symtab[list[0]] = address;
            Program[i].instruction = "RESB";
            Program[i].parameter = list[2];
            address += stoi(list[2]);
            Program[i].f = 0;
            i++;
        }
        else if (list[1] == "WORD")
        {
            Program[i].address = address;
            Program[i].label = list[0];
            symtab[list[0]] = address;
            Program[i].instruction = "WORD";
            Program[i].parameter = '\0';
            Program[i].objectCode = stoi(list[2]);
            Program[i].opcode = intToString(stoi(list[2]), 2);
            address += 3;
            Program[i].f = 3;
            i++;
        }
        else if (list[1] == "BYTE")
        {
            Program[i].address = address;
            Program[i].label = list[0];
            symtab[list[0]] = address;
            Program[i].instruction = "BYTE";
            Program[i].parameter = '\0';
            Program[i].objectCode = strhextoint(list[3]);
            Program[i].opcode = list[3];
            address += 1;
            Program[i].f = 1;
            i++;
        }
        else if (list[1] == "EQU")
        {
            Program[i].label = list[0];

            Program[i].instruction = "EQU";
            Program[i].parameter = '\0';
            Program[i].f = 3;
            if (list[2] == "*")
            {
                Program[i].objectCode = address;
                Program[i].address = address;
                symtab[list[0]] = address;
            }
            else
            {
                Program[i].objectCode = stoi(list[2]);
                Program[i].address = stoi(list[2]);
                symtab[list[0]] = stoi(list[2]);
            }
            i++;
        }

        else if (list[1] != "RESB" && list[1] != "RESW" && list[1] != "EQU" && list[1] != "WORD" && list[1] != "LTORG" && list[1] != "BYTE")
        {
            if (i != -1 && count == 3)
            {

                if (list[0][0] == '+')
                {
                    Program[i].address = address;
                    address += 4;
                    Program[i].f = 4;
                    Program[i].e = 1;
                    if (opcode.find(list[0].substr(1)) == opcode.end())
                    {
                        cout << "Invalid Instruction " << endl;
                        exit(1);
                    }
                    else
                    {
                        Program[i].instruction = list[0].substr(1);
                    }
                }
                else
                {
                    Program[i].address = address;
                    address += format[list[0]];
                    Program[i].f = format[list[0]];
                    if (opcode.find(list[0]) == opcode.end())
                    {
                        cout << "Invalid Instruction " << endl;
                        exit(1);
                    }
                    else
                    {
                        Program[i].instruction = list[0];
                    }
                }

                Program[i].label = "\0";
                Program[i].parameter = list[1];
                if (Program[i].parameter[0] == '=')
                {
                    littab.insert(Program[i].parameter);
                }
                i++;
            }
            else if (i != -1 && count == 4)
            {
                if (symtab.find(list[0]) == symtab.end())
                {
                    symtab[list[0]] = address;
                }
                else
                {
                    cout << "Duplicate Symbol " << endl;
                    exit(1);
                }

                if (list[1][0] == '+')
                {
                    Program[i].address = address;
                    address += 4;
                    Program[i].f = 4;
                    Program[i].e = 1;
                    if (opcode.find(list[1].substr(1)) == opcode.end())
                    {
                        cout << "Invalid Instruction " << endl;
                        exit(1);
                    }
                    else
                    {
                        Program[i].instruction = list[1].substr(1);
                    }
                }
                else
                {
                    Program[i].address = address;
                    Program[i].f = format[list[1]];
                    address += format[list[1]];
                    if (opcode.find(list[1]) == opcode.end())
                    {
                        cout << "Invalid Instruction " << endl;
                        exit(1);
                    }
                    else
                    {
                        Program[i].instruction = list[1];
                    }
                }

                Program[i].label = list[0];
                Program[i].parameter = list[2];
                if (Program[i].parameter[0] == '=')
                {
                    littab.insert(Program[i].parameter);
                }
                i++;
            }
            else if (i != -1 && count == 2)
            {
                Program[i].label = "\0";

                if (list[0][0] == '+')
                {
                    Program[i].address = address;
                    address += 4;
                    Program[i].f = 4;
                    Program[i].e = 1;
                    if (opcode.find(list[0].substr(1)) == opcode.end())
                    {
                        cout << "Invalid Instruction " << endl;
                        exit(1);
                    }
                    else
                    {
                        Program[i].instruction = list[0].substr(1);
                    }
                }
                else
                {
                    Program[i].address = address;
                    address += format[list[0]];
                    Program[i].f = format[list[0]];
                    if (opcode.find(list[0]) == opcode.end())
                    {
                        cout << "Invalid Instruction " << endl;
                        exit(1);
                    }
                    else
                    {
                        Program[i].instruction = list[0];
                    }
                }

                Program[i].parameter = "\0";
                i++;
            }
        }
    }
    cout << "End Not Specified" << endl;
    exit(1); 
}

void SecondPass()
{

    for (int i = 0; i < Program.size(); i++)
    {
        if (Program[i].instruction == "RESW" || Program[i].instruction == "RESB" || Program[i].instruction == "\0" || Program[i].instruction == "BYTE" || Program[i].instruction == "WORD" || Program[i].instruction == "")
        {
            continue;
        }
        if (Program[i].instruction == "LLLL")
        {
            continue;
        }
        if (Program[i].instruction == "RSUB")
        {
            Program[i].opcode = "4F0000";
            continue;
        }
        if (Program[i].parameter[0] == '@')
        {
            Program[i].i = 0;
            Program[i].n = 1;
            Program[i].parameter = Program[i].parameter.substr(1, Program[i].parameter.length() - 1);
        }
        else if (Program[i].parameter[0] == '#')
        {
            int sum;
            Program[i].i = 1;
            Program[i].n = 0;
            Program[i].parameter = Program[i].parameter.substr(1, Program[i].parameter.length() - 1);
            if (symtab.find(Program[i].parameter) == symtab.end())
            {
                if (stoi(Program[i].parameter) < 4096)
                {
                    Program[i].objectCode = opcode[Program[i].instruction];
                    sum = (Program[i].objectCode / 16) * pow(16, 5);
                    sum += (Program[i].objectCode % 16 + Program[i].n * 2 + Program[i].i) * pow(16, 4);
                    sum += stoi(Program[i].parameter);

                    Program[i].opcode = intToString(sum);

                    continue;
                }
            }
            else
            {
            }
        }
        else
        {
            Program[i].i = 1;
            Program[i].n = 1;
        }
        if (Program[i].parameter[Program[i].parameter.length() - 2] == ',' && Program[i].parameter[Program[i].parameter.length() - 1] == 'X')
        {
            Program[i].x = 1;

            Program[i].parameter = Program[i].parameter.substr(0, Program[i].parameter.length() - 2);
        }
        else
        {
            Program[i].x = 0;
        }
        if (Program[i].f == 2)
        {
            Program[i].objectCode = opcode[Program[i].instruction];
            Program[i].opcode += intToString(Program[i].objectCode, 2);
            Program[i].opcode += regcode[Program[i].parameter[0]];

            if (Program[i].parameter[2])
            {

                Program[i].opcode += regcode[Program[i].parameter[2]];
            }
            else
            {
                Program[i].opcode += '0';
            }
        }
        if (Program[i].f == 3)
        {
            int sum;
            Program[i].objectCode = opcode[Program[i].instruction];
            sum = (Program[i].objectCode / 16) * pow(16, 5);
            sum += (Program[i].objectCode % 16 + Program[i].n * 2 + Program[i].i) * pow(16, 4);

            if (abs(Program[i + 1].address - symtab[Program[i].parameter]) <= 4095)
            {
                Program[i].p = 1;
                Program[i].b = 0;
                sum += (Program[i].x * 8 + Program[i].b * 4 + Program[i].p * 2 + Program[i].e) * pow(16, 3);

                sum += (symtab[Program[i].parameter] - Program[i + 1].address);
                if (symtab[Program[i].parameter] - Program[i + 1].address < 0)
                {
                    sum += pow(16, 3);
                }
            }
            else if (base && (abs(symtab[Basearg] - symtab[Program[i].parameter]) <= 4095))
            {
                Program[i].p = 0;
                Program[i].b = 1;
                sum += (Program[i].x * 8 + Program[i].b * 4 + Program[i].p * 2 + Program[i].e) * pow(16, 3);
                sum += (symtab[Program[i].parameter] - symtab[Basearg]);
                if (symtab[Program[i].parameter] - symtab[Basearg] < 0)
                {
                    sum += pow(16, 3);
                }
            }
            else
            {
                cout <<  "Error Address out of bond" << endl;
                exit(1);
            }

            Program[i].opcode = intToString(sum);
        }
        if (Program[i].f == 4)
        {
            Program[i].p = 0;
            Program[i].b = 0;
            long sum = 0;
            Program[i].objectCode = opcode[Program[i].instruction];
            sum = (Program[i].objectCode / 16) * pow(16, 7);
            sum += (Program[i].objectCode % 16 + Program[i].n * 2 + Program[i].i) * pow(16, 6);
            sum += pow(16, 5);
            sum += symtab[Program[i].parameter];
            Program[i].opcode = intToString(sum, 8);
        }
    }
}

void Print()
{
    system("rm output.txt");
    ofstream op("output.txt");
    op << "H^";
    int l = Prgname.length();
    for (int i = 0; i <= 5; i++)
    {
        if (i < l)
        {
            op << Prgname[i];
        }
        else
        {
            op << " ";
        }
    }
    op << "^" << intToString(straddr) << "^" << intToString(lastaddr) << endl;
    for (int i = 0; i < Program.size(); i++)
    {
        if (Program[i].instruction == "RESW" || Program[i].instruction == "RESB" || Program[i].instruction == "EQU" || Program[i].instruction == "\0")
        {
            continue;
        }
        else
        {
            int last = i;
            int count = 0;
            while (count + Program[last].f < 30)
            {
                if (Program[last].instruction == "RESW" || Program[last].instruction == "RESB" || Program[last].instruction == "EQU")
                    break;
                count += Program[last].f;
                last++;
            }
            op << "T^" << intToString(Program[i].address) << "^" << intToString(count, 2);
            while (i != last)
            {
                if (Program[i].instruction == "RESW" || Program[i].instruction == "RESB" || Program[i].instruction == "EQU")
                {

                    break;
                }
                if (Program[i].opcode != "")
                    op << "^" << Program[i].opcode;
                i++;
            }
            op << endl;
            i--;
        }
    }
    for (int i = 0; i < Program.size(); i++)
    {
        if ((Program[i].e == 1 || Program[i].f == 4) && Program[i].n != 0)
        {
            op << "M^" << intToString(Program[i].address + 1) << "^05" << endl;
        }
    }

    op << "E^" << intToString(straddr) << endl;

    op.close();
}

int main(int argc, char *argv[])
{
    regcode['A'] = '0';
    regcode['X'] = '1';
    regcode['L'] = '2';
    regcode['B'] = '3';
    regcode['S'] = '4';
    regcode['T'] = '5';
    regcode['F'] = '6';
    literaltable["=C\'EOF\'"].first = 3;
    literaltable["=C\'EOF\'"].second = "454F46";

    literaltable["=X\'05\'"].first = 1;
    literaltable["=X\'05\'"].second = "05";

    optab();
    firstPass(argv[1]);
    SecondPass();
    Print();
}
