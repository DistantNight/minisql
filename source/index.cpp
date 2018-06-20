#include "index.h"
#include "buffer.h"
#include "catalog.h"
#include "OperationInput.h"
#include "BPTree.h"
#include <string>
#include <vector>
#include <map>

template <typename T> 
IndexManager<T>::IndexManager(const Table& table)
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
		if(indexs[i] != "") createIndex(indexs[i], types[i], sizes[i]);			
	}
	//}	
} 

template <typename T> 
IndexManager<T>::~IndexManager()
{
	for(intMap::iterator i_Int = indexIntMap.begin(); i_Int != indexIntMap.end(); ++i_Int)
	{
		if(i_Int -> second)
		{
			i_Int -> second -> writeBack();
			delete i_Int -> second;
		}
	}
	for(floatMap::iterator i_Float = indexFloatMap.begin(); i_Float != indexFloatMap.end(); ++i_Float)
	{
		if(i_Float -> second)
		{
			i_Float -> second -> writeBack();
			delete i_Float -> second;
		}
	}
	for(stringMap::iterator i_String = indexStringMap.begin(); i_String != indexStringMap.end(); ++i_String)
	{
		if(i_String -> second)
		{
			i_String -> second -> writeBack();
			delete i_String -> second;
		}
	}	
}

template <typename T> 
void IndexManager<T>::createIndex(std::string file_path, COLUMNTYPE type, int string_len)
{
	int degree = getBestDegree(type, string_len);
	
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

template <typename T> 
void IndexManager<T>::dropIndex(std::string file_path, COLUMNTYPE type)
{
	if(type == INT)
	{
		intMap::iterator i_Int = indexIntMap.find(file_path);
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
		floatMap::iterator i_Float = indexFloatMap.find(file_path);
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
		stringMap::iterator i_String = indexStringMap.find(file_path);
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

template <typename T> 
int IndexManager<T>::findIndex(std::string file_path, T data, COLUMNTYPE type)
{
	if(type == INT)
	{
		intMap::iterator i_Int = indexIntMap.find(file_path);
		if(i_Int == indexIntMap.end())
		{
			return -1;
		}
		else return i_Int -> second -> searchValueWithKey(data);
	}
	else if(type == FLOAT)
	{
		floatMap::iterator i_Float = indexFloatMap.find(file_path);
		if(i_Float == indexFloatMap.end())
		{
			return -1;
		}
		else return i_Float -> second -> searchValueWithKey(data);
	}
	else
	{
		stringMap::iterator i_String = indexStringMap.find(file_path);
		if(i_String == indexStringMap.end())
		{
			return -1;
		}
		else return i_String -> second -> searchValueWithKey(data);
	}
}

template <typename T>
void IndexManager<T>::insertIndex(std::string file_path, T data, COLUMNTYPE type, int block_id)
{
	if(type == INT)
	{
		intMap::iterator i_Int = indexIntMap.find(file_path);
		if(i_Int == indexIntMap.end())
		{
			return;
		}
		else i_Int -> second -> insertKey(data, block_id);
	}
	else if(type == FLOAT)
	{
		floatMap::iterator i_Float = indexFloatMap.find(file_path);
		if(i_Float == indexFloatMap.end())
		{
			return;
		}
		else i_Float -> second -> insertKey(data, block_id);
	}
	else
	{
		stringMap::iterator i_String = indexStringMap.find(file_path);
		if(i_String == indexStringMap.end())
		{
			return;
		}
		else i_String -> second -> insertKey(data, block_id);
	}
	return;
}

template <typename T>
void IndexManager<T>::deleteIndexByKey(std::string file_path, T data, COLUMNTYPE type)
{
	if(type == INT)
	{
		intMap::iterator i_Int = indexIntMap.find(file_path);
		if(i_Int == indexIntMap.end())
		{
			return;
		}
		else i_Int -> second -> deleteKey(data);
	}
	else if(type == FLOAT)
	{
		floatMap::iterator i_Float = indexFloatMap.find(file_path);
		if(i_Float == indexFloatMap.end())
		{
			return;
		}
		else i_Float -> second -> deleteKey(data);
	}
	else
	{
		stringMap::iterator i_String = indexStringMap.find(file_path);
		if(i_String == indexStringMap.end())
		{
			return;
		}
		else i_String -> second -> deleteKey(data);
	}
	return;
}

template <typename T> 
int IndexManager<T>::getBestDegree(COLUMNTYPE type, int string_len)
{	
	int degree = (BLOCK_LEN - sizeof(int)) / (string_len + sizeof(int));
	if(degree % 2 == 0) degree--;
	return degree;
}

//给定文件名，数据类型的范围查找，dataA与dataB大小关系无所谓，flag = 0是默认，1是无视dataA，返回>dataB的结果，2是无视dataB 
template <typename T>
void IndexManager<T>::searchWithinRange(std::string file_path, T dataA, T dataB, COLUMNTYPE type, std::vector<int>& values, int flag)
{
    if(type == INT) 
	{
        intMap::iterator itInt = indexIntMap.find(file_path);
        if(itInt == indexIntMap.end()) return;
        else itInt -> second -> searchInRange(dataA, dataB, values, flag);
    } 
	else if(type == FLOAT) 
	{
        floatMap::iterator itFloat = indexFloatMap.find(file_path);
        if (itFloat == indexFloatMap.end()) return;
        else itFloat -> second -> searchInRange(dataA, dataB, values, flag);
    } 
	else 
	{
        stringMap::iterator itString = indexStringMap.find(file_path);
        if(itString == indexStringMap.end()) return;
        else itString -> second -> searchInRange(dataA, dataB, values, flag);
    }
}

