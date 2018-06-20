#include "API.h"
#include "catalog.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>

using namespace std;
class catalogManager;
extern catalogManager x;

bool getTableInfo(Table & a)
{

    x.myprint(a.table_name);
    return true;
}

string catalogExecute(CreateTable & b)
{
    vector<string> colname;
    colname.reserve(b.column_num);
    for (int i = 0; i < b.column_num; i++)
    {
        colname.push_back(b.column_name[i]);
    }
    vector<string> coltype;
    for (int i = 0; i < b.column_num; i++)
    {
        //coltype.push_back(b.column_type[i]);
        if (b.column_type[i] == INT)
        {
            coltype.emplace_back("int");
        }
        else if (b.column_type[i] == CHAR)
        {
            //coltype.push_back("char");
            const int mx = b.string_length[i];
            const string my = "char(";
            char mmx[32];
            itoa(mx, mmx, 10);
            const string mn = mmx;
            const string s3 = ")";
            const string s4 = my + mn + s3;
            coltype.push_back(s4);
        }
        else if (b.column_type[i] == FLOAT)
        {
            coltype.emplace_back("float");
        }
    }
    vector<int> isU;
    isU.reserve(b.column_num);
    for (int i = 0; i < b.column_num; i++)
    {
        isU.push_back(b.is_unique[i]);
    }
    bool j = x.createTable(b.table_name, colname, coltype, isU, b.primary_key);
    if (j == 0)
    {

        return "create fail";
    }
    const string c = b.table_name + "_idx";
    x.createIndex(c, b.table_name, b.primary_key);

    return "creatr success";
}

string catalogExecute(DropTable & c)
{
    const int judge = x.isTableExist(c.table_name);
    if (judge == 0)
    {
        return "ERROR: table doesnt exist";
    }
    for (auto s = x.myt.begin(); s != x.myt.end(); ++s)
    {
        if ((*s).nameOfTable == c.table_name)
        {
            c.column_num = (*s).numberOfKey;
            for (int i = 0; i < (*s).numberOfKey; i++)
            {
                c.column_name[i] = (*s).nameOfKey[i];
            }
            for (int i = 0; i < (*s).numberOfKey; i++)
            {
                string str = (*s).nameOfType[i];
                const char * ee = str.c_str();
                if ((*s).nameOfType[i] == "int")
                {
                    c.column_type[i] = INT;
                }
                else if (strcspn(ee, "char") == 0)
                {
                    c.column_type[i] = CHAR;
                }
                else if ((*s).nameOfType[i] == "float")
                {
                    c.column_type[i] = FLOAT;
                }
            }
            for (int i = 0; i < (*s).numberOfKey; i++)
            {
                if ((*s).nameOfType[i] == "int" || (*s).nameOfType[i] == "float")
                {
                    c.string_length[i] = 0;
                }
                else
                {
                    const int f = (*s).nameOfType[i].find_first_of("(");
                    const int e = (*s).nameOfType[i].find_first_of(")");
                    string fen = (*s).nameOfType[i].substr(f + 1, e - 1);
                    int a = atoi(fen.c_str());
                    c.string_length[i] = a;
                }
            }
            for (int i = 0; i < (*s).numberOfKey; i++)
            {
                c.is_unique[i] = static_cast<bool>((*s).isUnique[i]);
            }
            c.primary_key = (*s).primaryKey;
            int numc = 0;
            for (int i = 0; i < (*s).numberOfKey; i++)
            {

                if ((*s).nameOfIndex[i] != "*") numc++;
            }
            c.exist_index_num = numc;
            for (int i = 0; i < (*s).numberOfKey; i++)
            {
                if ((*s).nameOfIndex[i] == "*")
                {
                    c.all_index_name[i] = "";
                }
                else
                {
                    c.all_index_name[i] = (*s).nameOfIndex[i];//* -> not exist
                }

            }


        }
        x.dropTable(c.table_name);

        return "drop success";
    }
    return "ERROR: table doesnt exist";
}

bool catalogExecute(CreateIndex &d)
{// need tablename indexname index_column_name

    int judge = x.isIndexExist(d.index_name, d.table_name);
    if (judge == 1)
    {

        cout << "index exist" << endl;
        return false;
    }
    x.createIndex(d.index_name, d.table_name, d.index_column_name);
    for (auto& s : x.myt)
    {
        if (s.nameOfTable == d.table_name)
        {
            d.column_num = s.numberOfKey;
            for (int i = 0; i < s.numberOfKey; i++)
            {
                d.column_name[i] = s.nameOfKey[i];
            }
            for (int i = 0; i < s.numberOfKey; i++)
            {
                string str = s.nameOfType[i];
                const char * ee = str.c_str();
                if (s.nameOfType[i] == "int")
                {
                    d.column_type[i] = INT;
                }
                else if (strcspn(ee, "char") == 0)
                {
                    d.column_type[i] = CHAR;
                }
                else if (s.nameOfType[i] == "float")
                {
                    d.column_type[i] = FLOAT;
                }
            }
            for (int i = 0; i < s.numberOfKey; i++)
            {
                if (s.nameOfType[i] == "int" || s.nameOfType[i] == "float")
                {
                    d.string_length[i] = 0;
                }
                else
                {
                    int f = s.nameOfType[i].find_first_of("(");
                    int e = s.nameOfType[i].find_first_of(")");
                    string fen = s.nameOfType[i].substr(f + 1, e - 1);
                    int a = atoi(fen.c_str());
                    d.string_length[i] = a;
                }
            }
            for (int i = 0; i < s.numberOfKey; i++)
            {
                d.is_unique[i] = static_cast<bool>(s.isUnique[i]);
            }
            d.primary_key = s.primaryKey;
            int numc = 0;
            for (int i = 0; i < s.numberOfKey; i++)
            {

                if (s.nameOfIndex[i] != "*") numc++;
            }
            d.exist_index_num = numc;
            for (int i = 0; i < s.numberOfKey; i++)
            {
                if (s.nameOfIndex[i] == "*")
                {
                    d.all_index_name[i] = "";
                }
                else
                {
                    d.all_index_name[i] = s.nameOfIndex[i];//* -> not exist
                }
            }


        }


    }
    cout << "create index successfully" << endl;
    return true;
}



bool catalogExecute(DropIndex &d)
{

    int judge = x.isIndexExist(d.index_name, d.table_name);
    if (judge = 0)
    {
        cout << "index not exist" << endl;
        return false;
    }
    for (auto& s : x.myt)
    {
        if (s.nameOfTable == d.table_name)
        {
            d.column_num = s.numberOfKey;
            for (int i = 0; i < s.numberOfKey; i++)
            {
                d.column_name[i] = s.nameOfKey[i];
            }
            for (int i = 0; i < s.numberOfKey; i++)
            {
                string str = s.nameOfType[i];
                const char * ee = str.c_str();
                if (s.nameOfType[i] == "int")
                {
                    d.column_type[i] = INT;
                }
                else if (strcspn(ee, "char") == 0)
                {
                    d.column_type[i] = CHAR;
                }
                else if (s.nameOfType[i] == "float")
                {
                    d.column_type[i] = FLOAT;
                }
            }
            for (int i = 0; i < s.numberOfKey; i++)
            {
                if (s.nameOfType[i] == "int" || s.nameOfType[i] == "float")
                {
                    d.string_length[i] = 0;
                }
                else
                {
                    int f = s.nameOfType[i].find_first_of("(");
                    int e = s.nameOfType[i].find_first_of(")");
                    string fen = s.nameOfType[i].substr(f + 1, e - 1);
                    int a = atoi(fen.c_str());
                    d.string_length[i] = a;
                }
            }
            for (int i = 0; i < s.numberOfKey; i++)
            {
                d.is_unique[i] = static_cast<bool>(s.isUnique[i]);
            }
            d.primary_key = s.primaryKey;
            int numc = 0;
            for (int i = 0; i < s.numberOfKey; i++)
            {

                if (s.nameOfIndex[i] != "*") numc++;
            }
            d.exist_index_num = numc;
            for (int i = 0; i < s.numberOfKey; i++)
            {
                if (s.nameOfIndex[i] == "*")
                {
                    d.all_index_name[i] = "";
                }
                else
                {
                    d.all_index_name[i] = s.nameOfIndex[i];//* -> not exist
                }
            }


        }


    }
    x.deleteIndex(d.index_name);

    cout << "delete successfully" << endl;
    return true;

}

bool catalogExecute(Insert &d)
{
    int j = x.isTableExist(d.table_name);
    if (j == 0)
    {

        cout << "insert error" << endl;
        return false;
    }

    for (auto& s : x.myt)
    {
        if (s.nameOfTable == d.table_name)
        {
            if (d.value_num != s.numberOfKey)
            {
                cout << "insert error" << endl;
                return false;
            }
            d.column_num = s.numberOfKey;
            for (int i = 0; i < s.numberOfKey; i++)
            {
                d.column_name[i] = s.nameOfKey[i];
            }
            for (int i = 0; i < s.numberOfKey; i++)
            {
                string str = s.nameOfType[i];
                const char * ee = str.c_str();
                if (s.nameOfType[i] == "int")
                {
                    d.column_type[i] = INT;
                }
                else if (strcspn(ee, "char") == 0)
                {
                    d.column_type[i] = CHAR;
                }
                else if (s.nameOfType[i] == "float")
                {
                    d.column_type[i] = FLOAT;
                }
            }
            for (int i = 0; i < s.numberOfKey; i++)
            {
                if (s.nameOfType[i] == "int" || s.nameOfType[i] == "float")
                {
                    d.string_length[i] = 0;
                }
                else
                {
                    int f = s.nameOfType[i].find_first_of("(");
                    int e = s.nameOfType[i].find_first_of(")");
                    string fen = s.nameOfType[i].substr(f + 1, e - 1);
                    d.string_length[i] = atoi(fen.c_str());
                }
            }
            for (int i = 0; i < s.numberOfKey; i++)
            {
                d.is_unique[i] = static_cast<bool>(s.isUnique[i]);
            }
            d.primary_key = s.primaryKey;
            int numc = 0;
            for (int i = 0; i < s.numberOfKey; i++)
            {

                if (s.nameOfIndex[i] != "*") numc++;
            }
            d.exist_index_num = numc;
            for (int i = 0; i < s.numberOfKey; i++)
            {
                if (s.nameOfIndex[i] == "*")
                {
                    d.all_index_name[i] = ("");
                }
                else
                {
                    d.all_index_name[i] = (s.nameOfIndex[i]);//* -> not exist
                }
            }


        }


    }
    return x.numberOfRecordAdd(d.table_name, 1);
}
bool catalogExecute(Delete &d)
{
    const int a = x.isTableExist(d.table_name);
    if (a == 0)
    {
        cout << "no table" << endl;
        return false;
    }
    for (auto& s : x.myt)
    {
        if (s.nameOfTable == d.table_name)
        {
            d.column_num = s.numberOfKey;
            for (int i = 0; i < s.numberOfKey; i++)
            {
                d.column_name[i] = s.nameOfKey[i];
            }
            for (int i = 0; i < s.numberOfKey; i++)
            {
                string str = s.nameOfType[i];
                const char * ee = str.c_str();
                if (s.nameOfType[i] == "int")
                {
                    d.column_type[i] = INT;
                }
                else if (strcspn(ee, "char") == 0)
                {
                    d.column_type[i] = CHAR;
                }
                else if (s.nameOfType[i] == "float")
                {
                    d.column_type[i] = FLOAT;
                }
            }
            for (int i = 0; i < s.numberOfKey; i++)
            {
                if (s.nameOfType[i] == "int" || s.nameOfType[i] == "float")
                {
                    d.string_length[i] = 0;
                }
                else
                {
                    const int f = s.nameOfType[i].find_first_of("(");
                    const int e = s.nameOfType[i].find_first_of(")");
                    string fen = s.nameOfType[i].substr(f + 1, e - 1);
                    d.string_length[i] = atoi(fen.c_str());
                }
            }
            for (int i = 0; i < s.numberOfKey; i++)
            {
                d.is_unique[i] = static_cast<bool>(s.isUnique[i]);
            }
            d.primary_key = s.primaryKey;
            int numc = 0;
            for (int i = 0; i < s.numberOfKey; i++)
            {

                if (s.nameOfIndex[i] != "*") numc++;
            }
            d.exist_index_num = numc;
            for (int i = 0; i < s.numberOfKey; i++)
            {
                if (s.nameOfIndex[i] == "*")
                {
                    d.all_index_name[i] = ("");
                }
                else
                {
                    d.all_index_name[i] = (s.nameOfIndex[i]);//* -> not exist
                }
            }


        }


    }
    if (x.deleteTableInfo(d.table_name, 1))
    {
        cout << "delete success" << endl;
        return true;
    }
    else
    {
        cout << "fail to delete" << endl;
        return false;
    }
}
bool catalogExecute(Select &d)
{
    const int a = x.isTableExist(d.table_name);
    if (a == 0)
    {
        cout << "table not exist" << endl;
        return false;
    }
    int flag = 0;
    for (auto& s : x.myt)
    {
        if (s.nameOfTable == d.table_name)
        {
            d.column_num = s.numberOfKey;
            for (int i = 0; i < s.numberOfKey; i++)
            {
                d.column_name[i] = s.nameOfKey[i];
            }
            for (int i = 0; i < s.numberOfKey; i++)
            {
                string str = s.nameOfType[i];
                const char * ee = str.c_str();
                if (s.nameOfType[i] == "int")
                {
                    d.column_type[i] = INT;
                }
                else if (strcspn(ee, "char") == 0)
                {
                    d.column_type[i] = CHAR;
                }
                else if (s.nameOfType[i] == "float")
                {
                    d.column_type[i] = FLOAT;
                }
            }
            for (int i = 0; i < s.numberOfKey; i++)
            {
                if (s.nameOfType[i] == "int" || s.nameOfType[i] == "float")
                {
                    d.string_length[i] = 0;
                }
                else
                {
                    const int f = s.nameOfType[i].find_first_of("(");
                    const int e = s.nameOfType[i].find_first_of(")");
                    string fen = s.nameOfType[i].substr(f + 1, e - 1);
 
                    d.string_length[i] = atoi(fen.c_str());
                }
            }
            for (int i = 0; i < s.numberOfKey; i++)
            {
                d.is_unique[i] = static_cast<bool>(s.isUnique[i]);
            }
            d.primary_key = s.primaryKey;
            int numc = 0;
            for (int i = 0; i < s.numberOfKey; i++)
            {

                if (s.nameOfIndex[i] != "*") numc++;
            }
            d.exist_index_num = numc;
            for (int i = 0; i < s.numberOfKey; i++)
            {
                if (s.nameOfIndex[i] == "*")
                {
                    d.all_index_name[i] = ("");
                }
                else
                {
                    d.all_index_name[i] = (s.nameOfIndex[i]);//* -> not exist
                }
            }


        }


    }

    vector<string> sr = x.nameOfKey(d.table_name);
    if (d.condition_name[0] == "*")
    {
        cout << "select success" << endl;
        return true;
    }
    for (const auto& i : sr)
    {
        if (d.condition_name[0] == i) flag = 1;
    }
    if (flag == 1)
    {
        cout << "select success" << endl;
        return true;
    }
    else
    {
        cout << "select error" << endl;
        return false;
    }
}