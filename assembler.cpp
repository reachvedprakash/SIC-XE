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
int straddr , lastaddr;
string Basearg;
int basevalue;
bool base;
vector<int> f4;
map<string, int> format, opcode, symtab;
map<string, pair<int, int> > literaltable;
set<string> littab;

class Ins
{
public:
    int address;
    string label;
    string instruction;
    string parameter;
    int objectCode;
    int e;
};
vector<Ins> Program(100);

string intToString(int n, int x = 6){										//function to convert int to string of default lenght 6
    stringstream stream;
    stream<<hex<<n;
    string result( stream.str() );
    for(int y=0;y<result.length();y++)	result[y] = toupper(result[y]);
    while(result.length()<x)	result = "0" + result;
    return result;
}


int strhextoint(string s)
{
    int sum = 0;
    int l = s.length();
    int i = 1;

    for (auto x : s)
    {
        // cout << sum << " " << l-i << endl;
        if (x == 'A')
        {
            sum += 10 * pow(16, l - i);
        }
        else if (x == 'B')
        {
            sum += 11 * pow(16, l - i);
        }
        else if (x == 'C')
        {
            sum += 12 * pow(16, l - i);
        }
        else if (x == 'D')
        {
            sum += 13 * pow(16, l - i);
        }
        else if (x == 'E')
        {
            sum += 14 * pow(16, l - i);
        }
        else if (x == 'F')
        {
            sum += 15 * pow(16, l - i);
        }
        else if (x <= 57 && x >= 48)
        {
            int i = (int)x;
            sum = i * pow(16, l - i);
        }
        else
        {
            cout << "Invalid hex" << endl;
            exit(1);
        }
        i++;
    }
    return sum;
}

void optab()
{
    // cout << "OPTAB Creation" << endl;
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
        // cout << ins << " " << frm << " " << hex << oc << endl;
        format[ins] = frm;
        opcode[ins] = oc;
    }
}

void firstPass(char *filename)
{
    string str;
    int address = 0;
    fstream fp(filename);
    int i = 1;
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
            // cout << sub <<  "\t" << count << endl;
        }
        if (list[0][0] == '.')
        {
            continue;
        }
        else if (list[0] == "END")
        {
            if(!littab.empty())
            {
                for (auto x : littab)
                {
                symtab[x] = address;
                Program[i].address = address;
                Program[i].label = x;
                Program[i].instruction = '\0';
                Program[i].parameter = x;
                Program[i].objectCode = literaltable[x].second;
                address += literaltable[x].first;
                // cout << Program[i].address << " " << Program[i].label << " " << Program[i].instruction << " " << Program[i].parameter << " " << Program[i].objectCode << endl;
                i++;
                }
                littab.clear();

            }
            return;
        }
        else if (list[0] == "BASE")
        {
            Basearg = list[1];
            base = true;
            continue;
        }
        else if (i == 1 && list[1] == "START")
        {
            Prgname = list[0];
            // cout << Prgname << endl ;
            straddr = strhextoint(list[2]);
            // cout << straddr << endl;
            address = straddr;
            i++;
        }
        else if (list[0] == "LTORG")
        {
            // cout << "\tLTORG" << endl;
            for (auto x : littab)
            {
                symtab[x] = address;
                Program[i].address = address;
                Program[i].label = x;
                Program[i].instruction = '\0';
                Program[i].parameter = x;
                Program[i].objectCode = literaltable[x].second;
                address += literaltable[x].first;
                // cout << Program[i].address << " " << Program[i].label << " " << Program[i].instruction << " " << Program[i].parameter << " " << Program[i].objectCode << endl;
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
            // cout << Program[i].address << " " << Program[i].label << " " << Program[i].instruction << " " << Program[i].parameter << " " << Program[i].objectCode << endl;
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
            // cout << Program[i].address << " " << Program[i].label << " " << Program[i].instruction << " " << Program[i].parameter << " " << Program[i].objectCode << endl;
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
            address += stoi(list[2]);
            // cout << Program[i].address << " " << Program[i].label << " " << Program[i].instruction << " " << Program[i].parameter << " " << Program[i].objectCode << endl;
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
            address += 1;
            // cout << Program[i].address << " " << Program[i].label << " " << Program[i].instruction << " " << Program[i].parameter << " " << Program[i].objectCode << endl;
            i++;
        }
        else if (list[1] == "EQU")
        {
            Program[i].label = list[0];
            
            Program[i].instruction = "EQU";
            Program[i].parameter = '\0';
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
            // cout << Program[i].address << " " << Program[i].label << " " << Program[i].instruction << " " << Program[i].parameter << " " << Program[i].objectCode << endl;
            i++;
        }

        else if (list[1] != "RESB" && list[1] != "RESW" && list[1] != "EQU" && list[1] != "WORD" && list[1] != "LTORG" && list[1] != "BYTE")
        {
            if (i != 1 && count == 3)
            {

                if (list[0][0] == '+')
                {
                    Program[i].address = address;
                    address += 4;
                    f4.push_back(Program[i].address);
                    Program[i].e = 1;
                    if (opcode.find(list[0].substr(1)) == opcode.end())
                    {
                        cout << "Invalid Instruction " << endl;
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
                    if (opcode.find(list[0]) == opcode.end())
                    {
                        cout << "Invalid Instruction " << endl;
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
                // cout << Program[i].address << "  " << Program[i].instruction << " " << Program[i].parameter << " " << Program[i].e << endl;
                i++;
            }
            else if (i != 1 && count == 4)
            {
                if (symtab.find(list[0]) == symtab.end())
                {
                    symtab[list[0]] = address;
                }
                else
                {
                    cout << "Duplicate Symbol " << endl;
                }

                if (list[1][0] == '+')
                {
                    Program[i].address = address;
                    address += 4;
                    f4.push_back(Program[i].address);
                    Program[i].e = 1;
                    if (opcode.find(list[1].substr(1)) == opcode.end())
                    {
                        cout << "Invalid Instruction " << endl;
                    }
                    else
                    {
                        Program[i].instruction = list[1].substr(1);
                    }
                }
                else
                {
                    Program[i].address = address;
                    address += format[list[1]];
                    if (opcode.find(list[1]) == opcode.end())
                    {
                        cout << "Invalid Instruction " << endl;
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
                // cout << Program[i].address << "  " << Program[i].label << " " << Program[i].instruction << " " << Program[i].parameter << " " << Program[i].e << endl;
                i++;
            }
            else if (i != 1 && count == 2)
            {
                Program[i].label = "\0";

                if (list[0][0] == '+')
                {
                    Program[i].address = address;
                    address += 4;
                    f4.push_back(Program[i].address);
                    Program[i].e = 1;
                    if (opcode.find(list[0].substr(1)) == opcode.end())
                    {
                        cout << "Invalid Instruction " << endl;
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
                    if (opcode.find(list[0]) == opcode.end())
                    {
                        cout << "Invalid Instruction " << endl;
                    }
                    else
                    {
                        Program[i].instruction = list[0];
                    }
                }

                Program[i].parameter = "\0";
                // cout << Program[i].address << "  " << Program[i].instruction << " " << Program[i].parameter << " " << Program[i].e << endl;
                i++;
            }
        }
        lastaddr = address ; 
    }
    return;
}

void SecondPass()
{
    // cout << "Hii!" << endl;
    int i = 1;
    // string s = ;
    fstream op("output.txt");
    op << "H^" ;

    for(int i = 0 ; i<= 5 ; i++)
    {
        int l = Prgname.length();
        if( i < l)
        {
            op << Prgname[i] ;
        } 
        else
        {
            op << " " ;
        }
        
    }
    op << "^"  << intToString(straddr) << endl;

    op.close();


}

int main(int argc, char *argv[])
{
    literaltable["=C\'EOF\'"].first = 3;
    literaltable["=C\'EOF\'"].second = strhextoint("454F46");

    literaltable["=X\'05\'"].first = 1;
    literaltable["=X\'05\'"].second = strhextoint("05");

    // for (auto x : literaltable)
    // {
    //     cout << x.first << " " << x.second.first << " " << x.second.second << endl;
    // }
    
    // cout << "Hello World" << endl;
    optab();
    firstPass(argv[1]);
    SecondPass();
    // for(auto x : symtab)
    // {
    //     cout << x.first << "\t" << x.second << endl;
    // }
}
