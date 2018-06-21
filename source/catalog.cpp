#include "catalog.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <utility>
using namespace std;

catalogManager::catalogManager() :tableNum(0), indexNum(0)
{
    string str;
    char temp[256];
    ifstream f("./data//catalog//db.info");
    if (!f)
    {
        cout << "CatalogManager ERROR------cant open db.catlog" << endl;
        exit(0);
    }

    f >> tableNum;
    f >> indexNum;
    if (tableNum != 0)
    {
        ifstream t("./data//catalog//table.info");
        if (!t)
        {
            cout << "CatalogManager ERROR------cant open table.catlog" << endl;
            exit(0);
        }
        t >> temp;
        while (strcmp(temp, "!end") != 0)
        {
            if (strcmp(temp, "!start") == 0)
            {
                t >> temp;

                catalogManager::table Ta;
                Ta.nameOfTable.assign(temp);
                t >> Ta.numberOfKey;
                for (int i = 0; i < Ta.numberOfKey; i++)
                {
                    t >> temp;
                    str.assign(temp);
                    Ta.nameOfKey.push_back(str);
                }
                for (int i = 0; i < Ta.numberOfKey; i++)
                {
                    t >> temp;
                    str.assign(temp);
                    Ta.nameOfType.push_back(str);
                }
                for (int i = 0; i < Ta.numberOfKey; i++)
                {
                    int x;
                    t >> x;
                    Ta.isUnique.push_back(x);
                }

                for (int i = 0; i < Ta.numberOfKey; i++)
                {
                    t >> temp;
                    str.assign(temp);
                    Ta.nameOfIndex.push_back(str);
                }
                int x;
                t >> x;
                Ta.numberOfRecord = x;
                Ta.lengthOfRecord = getFullLength(Ta.nameOfTable);
                t >> temp;
                if (strcmp(temp, "!end") == 0)
                {
                    myt.push_back(Ta);
                    break;
                }
                else if (strcmp(temp, "!start") == 0)
                {
                    myt.push_back(Ta);
                    continue;
                }
                Ta.primaryKey.assign(temp);

                myt.push_back(Ta);
            }
            t >> temp;
        }
        if (indexNum != 0)
        {
            ifstream i("./data//catalog//index.info");
            if (!i)
            {
                cout << "error------cant open index.catlog" << endl;
                exit(0);
            }
            i >> temp;
            while (strcmp(temp, "!end") != 0)
            {
                if (strcmp(temp, "!start") == 0)
                {
                    catalogManager::index In;
                    i >> temp;
                    In.nameOfIndex.assign(temp);
                    i >> In.tableOfIndex;
                    i >> In.keyOfIndex;
                    myi.push_back(In);
                }

                i >> temp;
            }
        }
    }
}


int catalogManager::isTableExist(const string& tablename)
{
    for (auto& s : myt)
    {
        if (s.nameOfTable == tablename) return 1;

    }
    return 0;
}

int catalogManager::isIndexExist(const string& indexname, const string& tablename)
{
    if (isTableExist(tablename) == 0) return 0;
    for (auto& s : myi)
    {
        if ((s.nameOfIndex == indexname) && (s.tableOfIndex == tablename)) return 1;

    }
    return 0;
}

int catalogManager::myprint(const string& tablename)
{
    if (isTableExist(tablename) == 0)
    {
        cout << "this table dosent exist" << endl;
        return 0;
    }
    else
    {
        for (auto& s : myt)
        {
            if (s.nameOfTable == tablename)
            {
                cout << "table's name:" << s.nameOfTable << endl;
                cout << "keys:";
                for (int i = 0; i < s.numberOfKey; i++)
                {
                    cout << s.nameOfKey[i] << " ";
                }cout << " " << endl;
                cout << "unique:";
                for (int i = 0; i < s.numberOfKey; i++)
                {
                    cout << s.isUnique[i] << " ";
                }cout << " " << endl;
                cout << "primary key:" << s.primaryKey << endl;
                cout << "number of record:" << s.numberOfRecord << endl;
                cout << "index:";
                for (int i = 0; i < s.numberOfKey; i++)
                {
                    cout << s.nameOfIndex[i] << " ";
                }
                cout << " " << endl;
            }

        }
    }
    return 1;
}
int catalogManager::deleteTableInfo(const string& nameOfTable)
{

    return deleteTableInfo(nameOfTable, getRecordNum(nameOfTable));

}
int catalogManager::deleteTableInfo(const string& nameOfTable, int d)
{
    
    for (auto& i : myt)
    {
        if (i.nameOfTable == nameOfTable)
        {
            i.numberOfRecord = i.numberOfRecord - d;
            return 1;
        }
    }
    return 0;
}
int catalogManager::dropTable(const string& nameOfTable)
{

    int flag = 0;
    tableNum--;
    for (auto p = myt.begin(); p != myt.end(); ++p)
    {
        if ((*p).nameOfTable == nameOfTable)
        {

            for (const auto& i : (*p).nameOfIndex)
                if (i != "*")
                    deleteIndex(i);

            p = myt.erase(p);
            flag = 1;
            break;
        }
    }
    if (flag == 0)
    {
        cout << nameOfTable << " doesnt exists" << endl;
        return 0;
    }

    remove(nameOfTable.c_str());


    return 1;
}

int catalogManager::createIndex(const string& nameOfIndex, const string& tableOfIndex, const string& keyOfIndex)
{
    vector<catalogManager::table>::iterator s;
    for (s = myt.begin(); s != myt.end(); ++s)
    {
        if ((*s).nameOfTable == tableOfIndex)
        {
            unsigned int i;
            for (i = 0; i < (*s).nameOfKey.size(); i++)
            {
                if ((*s).nameOfKey[i] == keyOfIndex)
                {

                    if (strcmp((*s).nameOfIndex[i].c_str(), "*") != 0)
                    {
                        cout << "The index already exists" << endl;
                        return 0;
                    }
                    (*s).nameOfIndex[i] = nameOfIndex;
                    break;
                }
            }
            if (i == (*s).nameOfKey.size())
            {
                cout << "Catalog Manager: Fail to create index" << endl;
                return 0;
            }
            break;
        }
    }
    if (s == myt.end())
    {
        cout << "Catalog Manager: Fail to create index" << endl;
        return 0;
    }


    catalogManager::index myt;
    myt.nameOfIndex = nameOfIndex;
    myt.tableOfIndex = tableOfIndex;
    myt.keyOfIndex = keyOfIndex;
    myi.push_back(myt);
    indexNum++;
    return 1;
}

int catalogManager::deleteIndex(const string& nameOfIndex)
{
    indexNum = indexNum - 1;
    int flag = 0;
    for (auto x = myi.begin(); x != myi.end(); ++x)
    {
        if ((*x).nameOfIndex == nameOfIndex)
        {
            for (auto& i : myt)
            {
                if (i.nameOfTable == (*x).tableOfIndex)
                {
                    for (unsigned int j = 0; j < i.nameOfIndex.size(); j++)
                    {
                        if (i.nameOfKey[j] == (*x).keyOfIndex)
                        {
                            i.nameOfIndex[j] = "*";
                            break;
                        }
                    }
                    break;
                }
            }

            x = myi.erase(x);
            flag = 1;
            break;
        }

    }
    if (flag != 1)
    {
        cout << nameOfIndex << " dosent exists" << endl;
    }
    remove(nameOfIndex.c_str());
    return 1;

}
int catalogManager::numberOfRecordAdd(const string& nameOfTable, int a)
{
    int flag = 0;
    for (auto& x : myt)
    {
        if (x.nameOfTable == nameOfTable)
        {
            flag = 1;
            x.numberOfRecord = x.numberOfRecord + a;
        }
    }
    if (flag == 0)
    {
        cout << "insert recorf fall" << endl;
        return 0;
    }
    return 1;
}


int catalogManager::updateCatalog()
{
    const char * fileName = "./data//catalog//db.info";
    ofstream file(fileName);
    if (!file)
    {
        cout << "cant write into db.info" << endl;
    }
    else
    {
        file << tableNum << endl;
        file << indexNum << endl;
    }
    const char * fileName2 = "./data//catalog//index.info";
    ofstream file2(fileName2);
    if (!file2)
    {
        cout << "cant write into index.info" << endl;
    }
    for (int i = 0; i < indexNum; i++)
    {
        file2 << "!start" << endl;
        file2 << myi[i].nameOfIndex << endl;
        file2 << myi[i].tableOfIndex << " " << myi[i].keyOfIndex << " " << endl;
    }   file2 << "!end" << endl;
    const char * fileName3 = "./data//catalog//table.info";
    ofstream file3(fileName3);
    if (!file3)
    {
        cout << "cant write into table.info" << endl;
    }
    for (int i = 0; i < tableNum; i++)
    {
        file3 << "!start" << endl;
        file3 << myt[i].nameOfTable << endl;
        file3 << myt[i].numberOfKey << endl;
        for (int j = 0; j < myt[i].numberOfKey; j++)
            file3 << myt[i].nameOfKey[j] << " ";
        file3 << endl;
        for (int j = 0; j < myt[i].numberOfKey; j++)
            file3 << myt[i].nameOfType[j] << " ";
        file3 << endl;
        for (int j = 0; j < myt[i].numberOfKey; j++)
            file3 << myt[i].isUnique[j] << " ";
        file3 << endl;
        for (int j = 0; j < myt[i].numberOfKey; j++)
            file3 << myt[i].nameOfIndex[j] << " ";
        file3 << endl;

        file3 << myt[i].numberOfRecord << endl;

        file3 << myt[i].primaryKey << endl;
    }
    file3 << "!end" << endl;

    return 0;
}
int catalogManager::getFullLength(const string& nameOfTable)
{
    int value = 0;
    vector<string> a;
    for (vector<catalogManager::table>::const_iterator b = myt.begin(); b != myt.end(); ++b)
    {
        if ((*b).nameOfTable == nameOfTable)
            a = (*b).nameOfType;
    }
    value = getLength(a);
    return value;
}
int catalogManager::getLength(vector<string> x) const
{
    int value = 0;
    for (vector<string>::const_iterator a = x.begin(); a != x.end(); ++a)
    {
        value = value + getLength(*a);
    }
    return value;
}
int catalogManager::getLength(const string& type) const
{
    const char * str = type.c_str();
    int x;
    if (strcspn(str, "int") == 0)
    {
        x = sizeof(int);
    }
    else if (strcspn(str, "char") == 0)
    {
        char p[5];
        const char * p1 = strstr(str, "(");
        const char *p2 = strstr(str, ")");
        p1++;
        strcpy(p, p1);
        p[p2 - p1] = 0;
        const int n = atoi(p);
        x = n * sizeof(char);
    }
    else if (strcspn(str, "float") == 0)
    {
        x = sizeof(float);
    }
    else
    {
        cout << "cant get length of type" << endl;
        return -1;
    }
    return x;




}

vector<string> catalogManager::nameOfKey(const string& nameOfTable)
{
    vector<string> a;
    for (vector<catalogManager::table>::const_iterator x = myt.begin(); x != myt.end(); ++x)
    {
        if ((*x).nameOfTable == nameOfTable)
            return (*x).nameOfKey;
    }
    return a;
}
vector<string> catalogManager::nameOfType(const string& nameOfTable)
{
    vector<string> a;
    for (vector<catalogManager::table>::const_iterator x = myt.begin(); x != myt.end(); ++x)
    {
        if ((*x).nameOfTable == nameOfTable)
            return (*x).nameOfType;
    }
    return a;
}
vector<int> catalogManager::isUnique(const string& nameOfTable)
{
    vector<int> a;
    for (vector<catalogManager::table>::const_iterator x = myt.begin(); x != myt.end(); ++x)
    {
        if ((*x).nameOfTable == nameOfTable)
            return (*x).isUnique;
    }
    return a;
}
vector<string> catalogManager::nameOfIndex(const string& nameOfTable)
{
    vector<string> a;
    for (vector<catalogManager::table>::const_iterator x = myt.begin(); x != myt.end(); ++x)
    {
        if (x->nameOfTable == nameOfTable)
            return x->nameOfIndex;
    }
    return a;
}

int catalogManager::getRecordNum(const string& tableName)
{
    int num = 0;
    for (auto& i : myt)
    {
        if (i.nameOfTable == tableName)
        {
            num = i.numberOfRecord;
            break;
        }
    }return num;
}
int catalogManager::createTable(const string& nameOfTable, const vector<string>& nameOfKey, const vector<string>&
                                nameOfType, vector<int> isUnique)
{
    const string x("*");
    if (createTable(nameOfTable, nameOfKey, nameOfType, std::move(isUnique), x)) return 1;
    else return 0;

}
int catalogManager::createTable(const string& nameOfTable, const vector<string>& nameOfKey, const vector<string>&
                                nameOfType, vector<int> isUnique, string primaryKey)
{
    if (isTableExist(nameOfTable) == 1)
    {
        cout << "this table has already exists" << endl;
        return 0;
    }
    else
    {
        tableNum++;
        catalogManager::table x;
        x.nameOfTable = nameOfTable;
        x.numberOfKey = nameOfKey.size();
        x.nameOfKey = nameOfKey;
        x.nameOfType = nameOfType;
        x.isUnique = std::move(isUnique);
        x.numberOfRecord = 0;
        x.lengthOfRecord = getLength(nameOfType);
        x.primaryKey = std::move(primaryKey);
        for (unsigned int i = 0; i < nameOfKey.size(); i++)
        {
            x.nameOfIndex.emplace_back("*");
        }
        myt.push_back(x);
        return 1;
    }


}
