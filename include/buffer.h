#ifndef _BUFFER_
#define _BUFFER_

#include <string>
#include <list>
#include <utility>
#include <vector>
#include <cstdint>

using std::string;

const int32_t MAX_FILE_ACTIVE = 8; // limit the active files in the buffer
const int32_t MAX_BLOCK = 2048; // the max number of the blocks
const int32_t BLOCK_LEN = 4096; // the size of one block

class Buffer;
class blockInfo;

class fileInfo
{
	bool type; // 0-> data file�� 1 -> index file
	string file_name; // the name of the file
	bool lock; // prevent the block from replacing
	std::list<blockInfo*> block_list; // point to the first block within the file

	friend class Buffer;

	fileInfo(string file_name, bool type) :
		type(type),
		file_name(std::move(file_name)),
		lock(false)
	{
	}
};

class blockInfo
{
	int32_t block_num; // the block number of the block which indicate it when it was built
	bool lock; // prevent the block from replacing
	fileInfo* file; // the pointer point to the file, which the block belong to

	friend class Buffer;

	blockInfo(int32_t block_num, fileInfo* file) :
		block_num(block_num),
		lock(false),
		file(file),
		dirty(false),
		char_num(0)
	{
		cBlock = new char[BLOCK_LEN];
		memset(cBlock, 0, BLOCK_LEN);
	}

	void block_reset(int32_t block_num, fileInfo* file); // reset block

	~blockInfo()
	{
		delete[] cBlock;
	}

private:
	bool dirty; // 0 -> false, 1 -> indicate dirty. write back
	//blockInfo* next; // the pointer point to next block
	int32_t char_num; // the number of chars int the block
	char* cBlock; // the array space for storing the records in the block in buffer
};
/*
template <typename T>
struct blockList // for LRU
{
	T* element;
	blockList* next;

	friend class Buffer;

	blockList() :
		element(nullptr),
		next(nullptr)
	{
	}
};
*/
class Buffer
{
private:
	std::list<fileInfo*> file_handle;
	std::list<blockInfo*> block_handle;
	std::list<blockInfo*> LRU_block_list;
	std::list<fileInfo*> LRU_file_list;

	int32_t total_block;
	int32_t total_file;

	static int count;

	/*
	function:
		��ȡ�������ݵ�block
	@param:
		block_node: ������ݵ��ڴ��
		file_type: �ļ����� 0->record, 1->index
		file_name: �����ļ���
	*/
	static void load_data(const string& file_name, bool file_type, blockInfo* block_node);

	/*
	function:
		��block�е�������д�ش���
	@param:
		block_node: ������ݵ��ڴ��
		file_type: �ļ����� 0->record, 1->index
		file_name: �����ļ���
	*/
	static void write_back(const string& file_name, bool file_type, blockInfo* block_node);

	/*
	function:
		�ر��ļ�
		���������д�ش���
		�ڴ��������������
		��LRU_block_list��LRU_file_list���޳���Ӧֵ
	@param
		file_node: ���رյ��ļ�ͷ
	*/
	void close_file(fileInfo* file_node);

public:

	Buffer() :
		total_block(0),
		total_file(0)
	{
	};

	~Buffer();

	/*
	function:
		�����ļ��Ƿ����ڴ��У�������ڣ������ļ�ͷ����������ڴ��ʼ���������ļ�ͷ
	@param:
		file_Name: �ļ���
		m_fileType: �ļ�����
	*/
	fileInfo* get_file_info(const string& file_name, bool file_type);

	/*
	function:
		�����ļ������ļ����Ͳ��Ҹ��ļ��Ƿ�����ڴ棬���ؿ�ָ��
	@param:
		file_Name: �ļ���
		fileType: �ļ����� 0->data, 1->index
		block_num: ���, ��Ŵ�0��ʼ
	*/
	blockInfo* get_file_block(const string& file_name, bool file_type, int block_num); //

	/*
	function:
		�����ļ�ͷָ�룬�������ļ������ڴ����滻
	@param:
		file_node: �ļ�ͷָ��
		lock: 0->unlock, 1->lock
	*/
	static void set_lock(fileInfo* file_node, bool lock);

	/*
	function:
		���ݿ�ָ�룬�����ÿ鲻���ڴ����滻
	@param:
		block_node: ��ָ��
		lock: 0->unlock, 1->lock
	*/
	static void set_lock(blockInfo* block_node, bool lock);

	/*
	function:
		���ݿ�ָ�룬�趨�ÿ��Ƿ��޸�
	@param:
		block_node: ��ָ��
		dirty: 1->dirty, 0->clean
	*/
	static void set_dirty(blockInfo* block_node, bool dirty);

	/*
	function:
		���ݿ�ָ�룬���ָ���ڴ�ռ��int8_tָ��
	@param:
		block_node: ��ָ��
	*/
	static char* get_content(blockInfo* block_node);

	/**
	 * \brief ���ݸ������ļ�����ɾ���ļ�
	 * \param file_name �ļ���
	 * \param file_type �ļ�����
	 */
	void remove_file(const string& file_name, bool file_type);

	void flash_blocks();
};

#endif
