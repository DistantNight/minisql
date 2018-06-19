#include "catalog.h"
#include<io.h>
#include<iostream>
#include<fstream>
using namespace std;

catalogManager::catalogManager() {
	string str;
	char temp[256];
	ifstream f("db.catalog");
	if (!f) {
		cout << "error------cant open db.catlog" << endl;
		exit(0);
	}
	
	f >> tableNum;
	f >> indexNum;
	if (tableNum != 0) {
		ifstream t("table.catalog");
		if (!t) {
			cout << "error------cant open table.catlog" << endl;
			exit(0);
		}
		t >> temp;
		while (strcmp(temp, "!end") != 0) {
			if (strcmp(temp, "!start") == 0 ) {
				t >> temp;
				
				catalogManager::table Ta;
				Ta.nameOfTable.assign(temp);
				t >> Ta.numberOfKey;
				for (int i = 0; i < Ta.numberOfKey; i++) {
					t >> temp;
					str.assign(temp);
					Ta.nameOfKey.push_back(str);
				}
				for (int i = 0; i < Ta.numberOfKey; i++) {
					t >> temp;
					str.assign(temp);
					Ta.nameOfType.push_back(str);
				}
				for (int i = 0; i < Ta.numberOfKey; i++) {
					int x;
					t >> x;
					Ta.isUnique.push_back(x);
				}
				
			for (int i = 0; i < Ta.numberOfKey; i++) {
					t >> temp;
					str.assign(temp);
					Ta.nameOfIndex.push_back(str);
				}
				int x;
				t >> x;
				Ta.numberOfRecord = x;
				Ta.lengthOfRecord = getLength(Ta.nameOfTable);
				t >> temp;
				Ta.primaryKey.assign(temp);


				myt.push_back(Ta);



			}

			t >> temp;
		}
		if (indexNum != 0) {
			ifstream i("index.catalog");
			if (!i) {
				cout << "error------cant open index.catlog" << endl;
				exit(0);
			}
			i >> temp;
			while (strcmp(temp, "!end") != 0) {
				if (strcmp(temp, "!start") == 0) {
					catalogManager::index In;
					i >> temp;
					In.nameOfIndex.assign(temp);
					i >> In.tableOfIndex;
					i >> In.keyOfIndex;

				}

				i >> temp;
			}
		}
	}



}



 
 int catalogManager::isTableExist(string tablename) {
	 vector<catalogManager::table>::iterator s;
	 for (s = myt.begin(); s != myt.end(); ++s) {
		 if ((*s).nameOfTable == tablename) return 1;
		 
	 }return 0;
 }
 int catalogManager::isIndexExist(string indexname) {
	 vector<catalogManager::index>::iterator s;
	 for (s = myi.begin(); s != myi.end(); ++s) {
		 if ((*s).nameOfIndex == indexname) return 1;

	 }return 0;
 }
 int catalogManager::myprint(string tablename) {
	 if (isTableExist(tablename) == 0) {
		 cout << "this table dosent exist" << endl;
		 return 0;
	 }
	 else {
		 vector<catalogManager::table>::iterator s;
		 for (s = myt.begin(); s != myt.end(); ++s) {
			 if ((*s).nameOfTable == tablename) {
				 cout << "table's name:" << (*s).nameOfTable << endl;
				 cout << "keys:";
					 for (int i = 0; i < (*s).numberOfKey; i++) {
						 cout << (*s).nameOfKey[i] << " ";
					 }cout << " " << endl;
					 cout << "unique:";
					 for (int i = 0; i < (*s).numberOfKey; i++) {
						 cout << (*s).isUnique[i] << " ";
					 }cout << " " << endl;
					 cout << "primary key:" << (*s).primaryKey << endl;
					 cout << "number of record:" << (*s).numberOfRecord << endl;
					 cout << "index:";
					 for (int i = 0; i < (*s).numberOfKey; i++) {
						 cout << (*s).nameOfIndex[i] << " ";
					 }cout << " " << endl;

			 }

		 }
	 }return 0;
 }
 bool catalogManager::deleteTableInfo(string nameOfTable){
 
	 return deleteTableInfo(nameOfTable, getRecordNum(nameOfTable));
 
 }
 bool catalogManager::deleteTableInfo(string nameOfTable,int d) {
	 int i = 0;
	 for (i; i < myt.size(); i++) {
		 if (myt[i].nameOfTable == nameOfTable) {
			 myt[i].numberOfRecord = myt[i].numberOfRecord - d;
			 return 1;
		 }
	 }
	 return 0;
 }
 bool catalogManager::dropTable(string nameOfTable) {

	 int flag = 0;
	 tableNum--;                                   
	 vector<catalogManager::table>::iterator p;
	 for (p = myt.begin(); p != myt.end(); ++p)
	 {
		 if ((*p).nameOfTable == nameOfTable)
		 {
			 
			 for (unsigned i = 0; i<(*p).nameOfIndex.size(); i++)
				 if ((*p).nameOfIndex.at(i) != "*")
					deleteIndex((*p).nameOfIndex.at(i));
			
			 p= myt.erase(p);
			 flag = 1;
			 break;
		 }
	 }
	 if (flag == 0)                
	 {
		 cout  << nameOfTable << " doesnt exists" << endl;
		 return 0;
	 }

	 remove(nameOfTable.c_str());


	 return 1;
 }

 bool catalogManager::createIndex(string nameOfIndex, string tableOfIndex, string keyOfIndex) {
	 vector<catalogManager::table>::iterator s;
	 for (s = myt.begin(); s != myt.end(); ++s)
	 {
		 if ((*s).nameOfTable== tableOfIndex)
		 {
			 unsigned int i = 0;
			 for (i = 0; i<(*s).nameOfKey.size(); i++)
			 {
				 if ((*s).nameOfKey[i] == keyOfIndex)
				 {
					 
					 if (strcmp((*s).nameOfIndex[i].c_str(), "*") != 0)
					 {
						 cout << "The index already exists" << endl;
						 return false;
					 }
					 (*s).nameOfIndex[i] = nameOfIndex;
					 break;
				 }
			 }
			 if (i == (*s).nameOfKey.size())
			 {
				 cout << "Fail to create index" << endl;
				 return false;
			 }
			 break;
		 }
	 }
	 if (s == myt.end())
	 {
		 cout << "Fail to create index" << endl;
		 return false;
	 }
	 
	 catalogManager::index myt;
	 myt.nameOfIndex = nameOfIndex;
	 myt.tableOfIndex = tableOfIndex;
	 myt.keyOfIndex = keyOfIndex;
	 myi.push_back(myt);
	 indexNum++;
     return true;	 
 }

 bool catalogManager::deleteIndex(const string& name_of_index) {
	 indexNum = indexNum - 1;
	 int flag = 0;
     for (auto x = myi.begin(); x != myi.end(); ++x) {
		 if ((*x).nameOfIndex == name_of_index) {
			 for (auto& i : myt)
			 {
				 if (i.nameOfTable == (*x).tableOfIndex) {
					 for (decltype(i.nameOfIndex.size()) j = 0; j < i.nameOfIndex.size(); j++) {
						 if (i.nameOfKey[j] == (*x).keyOfIndex) {
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
	 if (flag != 1) {
		 cout << name_of_index << " dosent exists" << endl;
}
	 remove(name_of_index.c_str());
	 return true;

 }
 bool catalogManager::numberOfRecordAdd(const string& name_of_table, int a) {
	 int flag = 0;
	 for (auto x = myt.begin(); x != myt.end(); ++x) {
		 if ((*x).nameOfTable == name_of_table) {
			 flag = 1;
			 (*x).numberOfRecord = (*x).numberOfRecord + a;
		 }
	 }
	 if (flag == 0) {
		 cout << "insert recorf fall" << endl;
		 return false;
	 }
	 return true;
 }
 

 bool catalogManager::updateCatalog() {
	const char * file_name = "db.catalog";
	ofstream file(file_name);
	if (!file) {
		cout << "cant write into db.catalog" << endl;
        return false;
	}
	else {
		file << tableNum << endl;
		file << indexNum << endl;
	}
	const char * file_name2 = "index.catalog";
	ofstream file2(file_name2);
	if (!file2) {
		cout << "cant write into index.catalog" << endl;
        return false;
	}
	for (int i = 0; i < indexNum; i++) {
		file2 << "!start" << endl;
		file2 << myi[i].nameOfIndex << endl;
		file2 << myi[i].tableOfIndex<<" "<<myi[i].keyOfIndex << " "<<endl;
	}   file2 << "!end" << endl;
	const char * file_name3 = "table.catalog";
	ofstream file3(file_name3);
	if (!file3) {
		cout << "cant write into table.catalog" << endl;
        return false;
	}
	for (int i = 0; i < tableNum; i++) {
		file3 << "!start" << endl;
		file3<< myt[i].nameOfTable << endl;
		file3 << myt[i].numberOfKey << endl;
		for (int j = 0; j<myt[i].numberOfKey; j++)
			file3 << myt[i].nameOfKey[j] << " ";
		file3 << endl;
		for (int j = 0; j<myt[i].numberOfKey; j++)
			file3 << myt[i].nameOfType[j] << " ";
		file3 << endl;
		for (int j = 0; j<myt[i].numberOfKey; j++)
			file3 << myt[i].isUnique[j] << " ";
		file3 << endl;
		for (int j = 0; j<myt[i].numberOfKey; j++)
			file3 << myt[i].nameOfIndex[j] << " ";
		file3 << endl;
		
		file3 << myt[i].numberOfRecord << endl;
		file3 << myt[i].lengthOfRecord << endl;
		file3 << myt[i].primaryKey<< endl;
	}
	file3 << "!end" << endl;
    return true;

 }
 int catalogManager::getFullLength(const string& name_of_table) {
     int value = 0;
	 vector<string> a;
	 for (auto b = myt.begin(); b != myt.end(); ++b) {
		 if ((*b).nameOfTable == name_of_table)
			 a = (*b).nameOfType;
	 }
	 value = getLength(a);

     return value;
 }
 int catalogManager::getLength(vector<string> x) {
	 int value = 0;
	 for (auto a = x.begin(); a != x.end(); ++a) {
		 value = value + getLength(*a);
	 }return value;
 }
 int catalogManager::getLength(string type) {
     const char * str = type.c_str();
	 int x;
	 if (strcspn(str, "int") == 0) {
		 x = sizeof(int);
	 }
	 else if (strcspn(str, "char") == 0) {
		 const char * p1, *p2;
		 char p[5];
		 int n;
		 p1 = strstr(str, "(");
		 p2 = strstr(str, ")");
		 p1++; 
		 strcpy(p, p1);
		 p[p2 - p1] = 0;
		 n = atoi(p);
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
 
 vector<string> catalogManager::nameOfKey(string nameOfTable) {
	 vector<catalogManager::table>::const_iterator x;
	 vector<string> a;
	 for (x = myt.begin(); x != myt.end(); x++) {
		 if ((*x).nameOfTable == nameOfTable)
			 return (*x).nameOfKey;
	 }return a;
 }
 vector<string> catalogManager::nameOfType(string nameOfTable) {
	 vector<catalogManager::table>::const_iterator x;
	 vector<string> a;
	 for (x = myt.begin(); x != myt.end(); x++) {
		 if ((*x).nameOfTable == nameOfTable)
			 return (*x).nameOfType;
	 }return a;
 }
 vector<int> catalogManager::isUnique(string nameOfTable) {
	 vector<catalogManager::table>::const_iterator x;
	 vector<int> a;
	 for (x = myt.begin(); x != myt.end(); x++) {
		 if ((*x).nameOfTable == nameOfTable)
			 return (*x).isUnique;
	 }return a;
 }
 vector<string> catalogManager::nameOfIndex(string nameOfTable) {
	 vector<catalogManager::table>::const_iterator x;
	 vector<string> a;
	 for (x = myt.begin(); x != myt.end(); x++) {
		 if ((*x).nameOfTable == nameOfTable)
			 return (*x).nameOfIndex;
	 }return a;
 }

 int catalogManager::getRecordNum(string tableName) {
	 int num = 0;
	 for (int i = 0; i < myt.size(); i++) {
		 if (myt.at(i).nameOfTable == tableName) {
			 num = myt.at(i).numberOfRecord;
			 break;
		 }
	 }return num;
 }
bool catalogManager::createTable(string nameOfTable, vector<string> nameOfKey, vector<string> nameOfType, vector<int> isUnique) {
	string x("*");
	if (createTable(nameOfTable, nameOfKey, nameOfType, isUnique, x)) return 1;
	else return 0;

}
bool catalogManager::createTable(string nameOfTable, vector<string> nameOfKey, vector<string> nameOfType, vector<int> isUnique, string primaryKey) {
	if (isTableExist(nameOfTable) == 1) {
		cout << "this table has already exists" << endl;
		return 0;
	}
	else {
		tableNum++;
		catalogManager::table x;
		x.nameOfTable = nameOfTable;
		x.numberOfKey = nameOfKey.size();
		x.nameOfKey = nameOfKey;
		x.nameOfType = nameOfType;
		x.isUnique = isUnique;
		x.numberOfRecord = 0;
		x.lengthOfRecord = getLength(nameOfType);
		x.primaryKey = primaryKey;
		for (int i = 0; i < nameOfKey.size(); i++) {
			x.nameOfIndex.push_back("*");
		}
		myt.push_back(x);
		return 1;
	}
		

}