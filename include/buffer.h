#ifndef _BUFFER_
#define _BUFFER_

#include <string>
#include <cstdint>

using std::string;

const int32_t MAX_FILE_ACTIVE = 5;	// limit the acctive files in the buffer
const int32_t MAX_BLOCK = 40;		// the max number of the blocks

struct fileInfo {
	bool type;				// 0-> data file�� 1 -> index file
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
		�����ѷ�����ڴ浥Ԫ�л�ȡ�����滻���ڴ��
		blockHandle -> LRU
	@param:
	*/
	blockInfo*	findBlock();

	/*
	function:
		��m_blockInfo��ָ��Ŀ����ӵ��ļ�ͷָ��m_fileInfo��ָ��Ŀ�����Ľ�β
		��m_blockInfo��ָ��Ŀ��fileָ��ָ��m_fileInfo
	@param:
		m_fileInfo:�ļ���Ϣ
		m_blockInfo:����Ϣ
	*/
	void replace(fileInfo* m_fileInfo, blockInfo* m_blockInfo);

	/*
	function:
	��ȡ���ļ���Ϣ
	@param:
		Table_Name: ����
	*/
	void get_table_info(string Table_Name); 


	/*
	function:
		��ȡ���ļ���Ϣ
	@param:
		Index_Name: ����
	*/
	void get_index_info(string Index_Name);

public:

	const int32_t BLOCK_LEN = 4096;		// the size of one block

	/*
	function:
		�����ļ��Ƿ����ڴ��У�������ڣ������ļ�ͷ����������ڴ��ʼ���������ļ�ͷ
	@param:
		file_Name: �ļ���
		m_fileType: �ļ�����
	*/
	fileInfo* get_file_Info(string file_Name, bool m_fileType);

	/*
	function:
		�����ļ������ļ����Ͳ��Ҹ��ļ��Ƿ�����ڴ棬���ؿ�ָ��
	@param:
		Table_Name: ����
		fileType: �ļ�����
		blockNum: ���
	*/
	blockInfo* get_file_block(string file_Name, int fileType, int blockNum);

	/*
	function:
		�����ļ�ͷָ�룬�������ļ������ڴ����滻
	@param:
		file_node: �ļ�ͷָ��
		lock: 0->unlock, 1->lock
	*/
	void set_lock(fileInfo &file_node, bool lock);

	/*
	function:
		���ݿ�ָ�룬�����ÿ鲻���ڴ����滻
	@param:
		block_node: ��ָ��
		lock: 0->unlock, 1->lock
	*/
	void set_lock(blockInfo &block_node, bool lock);

	/*
	function:
		���ݿ�ָ�룬�趨�ÿ��Ƿ��޸�
	@param:
		block_node: ��ָ��
		dirty: 1->dirty, 0->clean
	*/
	void set_dirty(blockInfo &block_node, bool dirty);
	
	/*
	function:
		���ݿ�ָ�룬���ָ���ڴ�ռ��int8_tָ��
	@param:
		block_node: ��ָ��
	*/
	int8_t* get_content(blockInfo &block_node);
};

#endif