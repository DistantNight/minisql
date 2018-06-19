#ifndef catalog_
#define catalog_



#include<string>
#include<vector>
#include "API.h"

using namespace std;

class catalogManager {

public:
	struct database {
		int numberOfIndex;
		int numberOfTable;

	};
	
	struct index {
		string nameOfIndex;
		string tableOfIndex;
		string keyOfIndex;
	};
	struct table {
		string nameOfTable;
		int numberOfKey;
		vector<string> nameOfKey;
		vector<string> nameOfType;
		vector<int> isUnique;
		
		vector<string> nameOfIndex;
		int numberOfRecord;
		int lengthOfRecord;
		string primaryKey;




	};
	vector<database> mydb;
	vector<table> myt;
	vector<index> myi;
	int tableNum;
	int indexNum;

	catalogManager();
	~catalogManager() {};
	int isTableExist(string tablename);
	int isIndexExist(string indexname);
	int myprint(string tablename);
	bool createTable(string nameOfTable, vector<string> nameOfKey, vector<string> nameOfType, vector<int> isUnique);
	bool createTable(string nameOfTable, vector<string> nameOfKey, vector<string> nameOfType, vector<int> isUnique, string primaryKey);
	bool deleteTableInfo(string nameOfTable);
	bool deleteTableInfo(string nameOfTable, int d);
	
	
	bool createIndex(string nameOfIndex, string tableOfIndex, string keyOfIndex);
	bool deleteIndex(const string& nameOfIndex);
	bool numberOfRecordAdd(const string& nameOfTable, int a);
	
	bool dropTable(string nameOfTable);
	bool updateCatalog(); // TODO: execute before quit
	int getFullLength(const string& nameOfTable);
	int getRecordNum(string tableName);
	int getLength(vector<string> x);
	int getLength(string type);
	
	vector<string> nameOfKey(string nameOfTable);
	vector<string> nameOfType(string nameOfTable);
	vector<int> isUnique(string nameOfTable);
	vector<string> nameOfIndex(string nameOfTable);



};



#endif // !