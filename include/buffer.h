#ifndef _BUFFER_
#define _BUFFER_

#include <string>
#include <cstdint>

using std::string;

const int32_t MAX_FILE_ACTIVE = 5;	// limit the acctive files in the buffer
const int32_t MAX_BLOCK = 40;		// the max number of the blocks

struct fileInfo {
	bool type;				// 0-> data file， 1 -> index file
	string fileName;		// the name of the file
	bool lock;				// prevent the block from replacing
	fileInfo *next;			// the pointer points to the next file
	blockInfo *firstBlock;	// point to the first block within the file
};

struct blockInfo
{
	int32_t blockNum;	// the block number of the block which indicate it when it was built
	bool lock;			// prevent the block from replacing
	fileInfo* file;		// the pointer point to the file, which the block belong to

private:
	bool dirtyBit;		// 0 -> false, 1 -> indicate dirty. write back
	blockInfo* next;	// the pointer point to next block
	blockInfo* pre;		// the pointer point to pre block
	int32_t charNum;	// the number of chars int the block
	int8_t *cBlock;		// the array space for storing the records in the block in buffer
	int32_t iTime;		// it indicate the age of the block in use
};

class Buffer {

private:
	fileInfo * file_handle;
	blockInfo * block_handle;

	int32_t total_block;
	int32_t total_file;

	/*
	function:
		从现已分配的内存单元中获取用于替换的内存块
		blockHandle -> LRU
	@param:
	*/
	blockInfo*	findBlock();

	/*
	function:
		将m_blockInfo所指向的块连接到文件头指针m_fileInfo所指向的块链表的结尾
		将m_blockInfo所指向的块的file指针指向m_fileInfo
	@param:
		m_fileInfo:文件信息
		m_blockInfo:块信息
	*/
	void replace(fileInfo* m_fileInfo, blockInfo* m_blockInfo);

	/*
	function:
	读取表文件信息
	@param:
		Table_Name: 表名
	*/
	void get_table_info(string Table_Name); 


	/*
	function:
		读取表文件信息
	@param:
		Index_Name: 表名
	*/
	void get_index_info(string Index_Name);

public:

	const int32_t BLOCK_LEN = 4096;		// the size of one block

	/*
	function:
		查找文件是否在内存中，如果存在，返回文件头，否则分配内存初始化，返回文件头
	@param:
		file_Name: 文件名
		m_fileType: 文件类型
	*/
	fileInfo* get_file_Info(string file_Name, bool m_fileType);

	/*
	function:
		根据文件名，文件类型查找该文件是否存在内存，返回块指针
	@param:
		Table_Name: 表名
		fileType: 文件类型
		blockNum: 块号
	*/
	blockInfo* get_file_block(string file_Name, int fileType, int blockNum);

	/*
	function:
		根据文件头指针，锁定该文件不从内存中替换
	@param:
		file_node: 文件头指针
		lock: 0->unlock, 1->lock
	*/
	void set_lock(fileInfo &file_node, bool lock);

	/*
	function:
		根据块指针，锁定该块不从内存中替换
	@param:
		block_node: 块指针
		lock: 0->unlock, 1->lock
	*/
	void set_lock(blockInfo &block_node, bool lock);

	/*
	function:
		根据块指针，设定该块是否被修改
	@param:
		block_node: 块指针
		dirty: 1->dirty, 0->clean
	*/
	void set_dirty(blockInfo &block_node, bool dirty);
	
	/*
	function:
		根据块指针，获得指向内存空间的int8_t指针
	@param:
		block_node: 块指针
	*/
	int8_t* get_content(blockInfo &block_node);
};

#endif