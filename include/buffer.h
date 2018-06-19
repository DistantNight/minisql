#ifndef _BUFFER_
#define _BUFFER_

#include <string>
#include <cstdint>

using std::string;

const int32_t MAX_FILE_ACTIVE = 5;	// limit the acctive files in the buffer
const int32_t MAX_BLOCK = 40;		// the max number of the blocks
const int32_t BLOCK_LEN = 4096;		// the size of one block

class Buffer;
class blockInfo;

class fileInfo {
	bool type;				// 0-> data file， 1 -> index file
	string file_name;		// the name of the file
	bool lock;				// prevent the block from replacing
	fileInfo *next;			// the pointer points to the next file
	blockInfo *first_block;	// point to the first block within the file

	friend class Buffer;

	fileInfo(string file_name, bool type) :
		file_name(file_name),
		type(type),
		lock(0),
		next(NULL),
		first_block(NULL) {}
};

class blockInfo {
	int32_t block_num;	// the block number of the block which indicate it when it was built
	bool lock;			// prevent the block from replacing
	fileInfo* file;		// the pointer point to the file, which the block belong to

	friend class Buffer;

	blockInfo(int32_t block_num, fileInfo* file) :
		block_num(block_num),
		lock(0),
		file(file), 
		dirty(0),
		next(NULL),
		char_num(0) {
		cBlock = new char[BLOCK_LEN];
		memset(cBlock, 0, BLOCK_LEN);
	}

	void block_reset(int32_t block_num, fileInfo* file); // reset block
	
	~blockInfo() {
		delete[] cBlock;
	}

private:
	bool dirty;			// 0 -> false, 1 -> indicate dirty. write back
	blockInfo* next;	// the pointer point to next block
	int32_t char_num;	// the number of chars int the block
	char *cBlock;		// the array space for storing the records in the block in buffer
};

template<typename T> struct blockList		// for LRU
{
	T* element;
	blockList* next;

	friend class Buffer;

	blockList() :
		next(NULL),
		element(NULL) {}
};

class Buffer {

private:
	fileInfo * file_handle;
	blockInfo * block_handle;
	blockList<blockInfo> * LRU_block_list;
	blockList<fileInfo> * LRU_file_list;

	int32_t total_block;
	int32_t total_file;

	/*
	function:
		读取磁盘数据到block
	@param:
		block_node: 存放数据的内存块
		file_type: 文件类型 0->record, 1->index
		file_name: 磁盘文件名
	*/
	void load_data(string file_name, bool file_type, blockInfo* block_node);

	/*
	function:
		将block中的脏数据写回磁盘
	@param:
		block_node: 存放数据的内存块
		file_type: 文件类型 0->record, 1->index
		file_name: 磁盘文件名
	*/
	void write_back(string file_name, bool file_type, blockInfo* block_node);

	/*
	function:
		关闭文件
		将脏块数据写回磁盘
		内存块链入垃圾链表
		从LRU_block_list与LRU_file_list中剔除相应值
	@param
		file_node: 待关闭的文件头
	*/
	void close_file(fileInfo* file_node);

public:

	Buffer() :
		total_block(0),
		total_file(0) {
		
		file_handle = new fileInfo("", 0);
		file_handle->lock = 1;
		block_handle = new blockInfo(-1, file_handle);
		block_handle->lock = 1;
		
		LRU_block_list = new blockList<blockInfo>;
		LRU_file_list = new blockList<fileInfo>;
		LRU_block_list->element = block_handle;
		LRU_block_list->next = NULL;
		LRU_file_list->element = file_handle;
		LRU_file_list->next = NULL;
	};

	~Buffer();

	/*
	function:
		查找文件是否在内存中，如果存在，返回文件头，否则分配内存初始化，返回文件头
	@param:
		file_Name: 文件名
		m_fileType: 文件类型
	*/
	fileInfo* get_file_info(string file_name, bool file_type);

	/*
	function:
		根据文件名，文件类型查找该文件是否存在内存，返回块指针
	@param:
		file_Name: 文件名
		fileType: 文件类型 0->data, 1->index
		block_num: 块号, 编号从0开始
	*/
	blockInfo* get_file_block(string file_name, bool file_type, int block_num); //

	/*
	function:
		根据文件头指针，锁定该文件不从内存中替换
	@param:
		file_node: 文件头指针
		lock: 0->unlock, 1->lock
	*/
	void set_lock(fileInfo *file_node, bool lock);

	/*
	function:
		根据块指针，锁定该块不从内存中替换
	@param:
		block_node: 块指针
		lock: 0->unlock, 1->lock
	*/
	void set_lock(blockInfo *block_node, bool lock);

	/*
	function:
		根据块指针，设定该块是否被修改
	@param:
		block_node: 块指针
		dirty: 1->dirty, 0->clean
	*/
	void set_dirty(blockInfo *block_node, bool dirty);
	
	/*
	function:
		根据块指针，获得指向内存空间的int8_t指针
	@param:
		block_node: 块指针
	*/
	char* get_content(blockInfo *block_node);

	/**
	 * \brief 根据给定的文件名，删除文件
	 * \param file_name 文件名
	 * \param file_type 文件类型
	 */
	void remove_file(string file_name, bool file_type);
};

#endif