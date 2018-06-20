#ifndef _INDEX_MANAGER_
#define _INDEX_MANAGER_

#include <map>
#include <sstream>
#include <fstream>
#include "OperationInput.h" 
#include "BPTree.h"
#include "buffer.h"

template <typename T> 
class IndexManager{
	public:
		IndexManager(const Table& table);
		~IndexManager();
		//建立索引
		//输入：索引名，属性类型，该类型长度 
		void 		createIndex(std::string file_path, COLUMNTYPE type, int string_len);
		//删除索引
		//输入：索引名，属性类型
		void 		dropIndex(std::string file_path, COLUMNTYPE type); 
		int 		findIndex(std::string file_path, T data, COLUMNTYPE type);
		void 		insertIndex(std::string file_path, T data, COLUMNTYPE type, int block_id);
		void 		deleteIndexByKey(std::string file_path, T data, COLUMNTYPE type);
		void 		searchWithinRange(std::string file_path, T dataA, T dataB, COLUMNTYPE type, std::vector<int>& values, int flag);
	
	private:
		typedef 	std::map<std::string, BPTree<int> *> 			intMap;
		typedef 	std::map<std::string, BPTree<float> *> 			floatMap;
		typedef 	std::map<std::string, BPTree<std::string> *> 	stringMap;
		
		intMap 		indexIntMap;
		floatMap 	indexFloatMap;
		stringMap 	indexStringMap;
		
		int 		getBestDegree(COLUMNTYPE type, int string_len);
};

#endif
