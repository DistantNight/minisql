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
		int numberOfKey = 0;
		vector<string> nameOfKey;
		vector<string> nameOfType;
		vector<int> isUnique;

		vector<string> nameOfIndex;
		int numberOfRecord = 0;
		int lengthOfRecord = 0;
		string primaryKey;




	};
	vector<database> mydb;
	vector<table> myt;
	vector<index> myi;
	int tableNum;
	int indexNum;

	catalogManager();
	~catalogManager() = default;
	int isTableExist(const string& tablename);
	int isIndexExist(const string& indexname, const string& tablename);
	int myprint(const string& tablename);
	int createTable(const string& nameOfTable, const vector<string>& nameOfKey, const vector<string>& nameOfType, vector<int> isUnique);
	int createTable(const string& nameOfTable, const vector<string>& nameOfKey, const vector<string>& nameOfType, vector<int> isUnique, string primaryKey);
	int deleteTableInfo(const string& nameOfTable);
	int deleteTableInfo(const string& nameOfTable, int d);


	int createIndex(const string& nameOfIndex, const string& tableOfIndex, const string& keyOfIndex);
	int deleteIndex(const string& nameOfIndex);
	int numberOfRecordAdd(const string& nameOfTable, int a);

	int dropTable(const string& nameOfTable);
	int updateCatalog();
	int getFullLength(const string& nameOfTable);
	int getRecordNum(const string& tableName);
	int getLength(vector<string> x) const;
	int getLength(const string& type) const;

	vector<string> nameOfKey(const string& nameOfTable);
	vector<string> nameOfType(const string& nameOfTable);
	vector<int> isUnique(const string& nameOfTable);
	vector<string> nameOfIndex(const string& nameOfTable);



};



#endif // !