#include "index.h"
#include "buffer.h"
#include "OperationInput.h"
#include "BPTree.h"
#include <string>
#include <vector>
#include <map>

IndexManager::IndexManager(const Table& table)
{
	//catalogManager catalog;
	//if(catalog.isTableExist(Table.table_name))
	//{
	//vector<std::string> indexs = catalog.nameOfIndex(table_name);
	//vector<std::string> types = catalog.nameOfType(table_name);
	const string* indexs = table.all_index_name;
	const ColumnType* types = table.column_type;
	const int* sizes = table.string_length;
	for(unsigned int i = 0; i < COLUMNMAXSIZE; i++)
	{
		if(!indexs[i].empty()) createIndex(indexs[i], types[i], sizes[i]);			
	}
	//}	
} 

IndexManager::~IndexManager()
{
	for (auto& i_Int : indexIntMap)
	{
		if(i_Int.second)
		{
		    i_Int.second -> writeBack();
			delete i_Int.second;
		}
	}
	for (auto& i_Float : indexFloatMap)
	{
		if(i_Float.second)
		{
		    i_Float.second->writeBack();
			delete i_Float.second;
		}
	}
	for (auto& i_String : indexStringMap)
	{
		if(i_String.second)
		{
		    i_String.second->writeBack();
			delete i_String.second;
		}
	}	
}

void IndexManager::createIndex(const std::string& file_path, COLUMNTYPE type, int string_len)
{
    const int degree = getBestDegree(type, string_len);
	
	if(type == INT)
	{
		BPTree<int> *BPT = new BPTree<int>(file_path, degree);
		indexIntMap.insert(intMap::value_type(file_path, BPT));
	}
	else if(type == FLOAT)
	{
		BPTree<float> *BPT = new BPTree<float>(file_path, degree);
		indexFloatMap.insert(floatMap::value_type(file_path, BPT));
	}
	else
	{
		BPTree<std::string> *BPT = new BPTree<std::string>(file_path, degree);
		indexStringMap.insert(stringMap::value_type(file_path, BPT));
	}
	return;
}

void IndexManager::dropIndex(const std::string& file_path, COLUMNTYPE type)
{
	if(type == INT)
	{
	    auto i_Int = indexIntMap.find(file_path);
		if(i_Int == indexIntMap.end())
		{
			return;
		}
		else
		{
			delete i_Int -> second;
			indexIntMap.erase(i_Int);
		}
	}
	else if(type == FLOAT)
	{
	    auto i_Float = indexFloatMap.find(file_path);
		if(i_Float == indexFloatMap.end())
		{
			return;
		}
		else
		{
			delete i_Float -> second;
			indexFloatMap.erase(i_Float);
		}
	}
	else
	{
	    auto i_String = indexStringMap.find(file_path);
		if(i_String == indexStringMap.end())
		{
			return;
		}
		else
		{
			delete i_String -> second;
			indexStringMap.erase(i_String);
		}
	}
	return;
}

int IndexManager::getBestDegree(COLUMNTYPE type, int string_len)
{
	int degree = (BLOCK_LEN - sizeof(int)) / (string_len + sizeof(int));
	if(degree % 2 == 0) degree--;
	return degree;
}

