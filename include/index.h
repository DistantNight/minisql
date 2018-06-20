#ifndef _INDEX_MANAGER_
#define _INDEX_MANAGER_

#include <map>
#include <sstream>
#include <fstream>
#include "OperationInput.h" 
#include "BPTree.h"
#include "buffer.h"

class IndexManager{
	public:
		IndexManager(const Table& table);
		~IndexManager();
		//建立索引
		//输入：索引名，属性类型，该类型长度 
		void 		createIndex(const std::string& file_path, COLUMNTYPE type, int string_len);
		//删除索引
		//输入：索引名，属性类型
		void 		dropIndex(const std::string& file_path, COLUMNTYPE type); 
		template 	<typename T> 
		int 		findIndex(const std::string& file_path, T data, COLUMNTYPE type);
		template 	<typename T> 
		void 		insertIndex(const std::string& file_path, T data, COLUMNTYPE type, int block_id);
		template 	<typename T> 
		void 		deleteIndexByKey(const std::string& file_path, T data, COLUMNTYPE type);
		template 	<typename T> 
		void 		searchWithinRange(const std::string& file_path, T dataA, T dataB, COLUMNTYPE type, std::vector<int>& values, int flag);
	
	private:
		typedef 	std::map<std::string, BPTree<int> *> 			intMap;
		typedef 	std::map<std::string, BPTree<float> *> 			floatMap;
		typedef 	std::map<std::string, BPTree<std::string> *> 	stringMap;
		
		intMap 		indexIntMap;
		floatMap 	indexFloatMap;
		stringMap 	indexStringMap;


		int 		getBestDegree(COLUMNTYPE type, int string_len);
};

template <typename T> 
int IndexManager::findIndex(const std::string& file_path, T data, COLUMNTYPE type)
{
	if(type == INT)
	{
		intMap::iterator i_Int = indexIntMap.find(file_path);
		if(i_Int == indexIntMap.end())
		{
			return -1;
		}
		else return i_Int -> second -> searchValueWithKey(reinterpret_cast<int&>(data));
	}
	else if(type == FLOAT)
	{
		floatMap::iterator i_Float = indexFloatMap.find(file_path);
		if(i_Float == indexFloatMap.end())
		{
			return -1;
		}
		else return i_Float -> second -> searchValueWithKey(reinterpret_cast<float&>(data));
	}
	else
	{
		stringMap::iterator i_String = indexStringMap.find(file_path);
		if(i_String == indexStringMap.end())
		{
			return -1;
		}
		else return i_String -> second -> searchValueWithKey(reinterpret_cast<string&>(data));
	}

}

template <typename T>
void IndexManager::insertIndex(const std::string& file_path, T data, COLUMNTYPE type, int block_id)
{
	if(type == INT)
	{
		intMap::iterator i_Int = indexIntMap.find(file_path);
		if(i_Int == indexIntMap.end())
		{
			return;
		}
		else i_Int -> second -> insertKey(reinterpret_cast<int&>(data), block_id);
	}
	else if(type == FLOAT)
	{
		floatMap::iterator i_Float = indexFloatMap.find(file_path);
		if(i_Float == indexFloatMap.end())
		{
			return;
		}
		else i_Float -> second -> insertKey(reinterpret_cast<float&>(data), block_id);
	}
	else
	{
		stringMap::iterator i_String = indexStringMap.find(file_path);
		if(i_String == indexStringMap.end())
		{
			return;
		}
		else i_String -> second -> insertKey(reinterpret_cast<string&>(data), block_id);
	}
    
}

template <typename T>
void IndexManager::deleteIndexByKey(const std::string& file_path, T data, COLUMNTYPE type)
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
 


//给定文件名，数据类型的范围查找，dataA与dataB大小关系无所谓，flag = 0是默认，1是无视dataA，返回>dataB的结果，2是无视dataB 
template <typename T>
void IndexManager::searchWithinRange(const std::string& file_path, T dataA, T dataB, COLUMNTYPE type, std::vector<int>& values, int flag)
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

#endif
