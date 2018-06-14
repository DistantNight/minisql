#include "buffer.h"
#include <fstream>
#include <cstdlib>

using std::fstream;
using std::ios;
const string ROOT = "./data/";

void blockInfo::block_reset(int32_t block_num, fileInfo* file) {
	this->block_num = block_num;
	memset(this->cBlock, 0, BLOCK_LEN);
	this->block_num = 0;
	this->dirty = 0;
	this->lock = 0;
	this->file = file;
}

fileInfo* Buffer::get_file_info(string file_name, bool file_type) {
	
	fileInfo* file_pre = NULL;
	fileInfo* iter = NULL;

	for (iter = file_handle; iter; iter = iter->next) {
		if (iter->file_name == file_name) {
			break;
		}
		file_pre = iter;
	}

	blockList<fileInfo>* temp = NULL;
	for (temp = LRU_file_list; temp->next; temp = temp->next);

	if (!iter) { //�ļ�δ��
		if (total_file < MAX_FILE_ACTIVE) { // ���Դ��ļ�
			file_pre->next = new fileInfo(file_name, 0);
			iter = file_pre->next;

			//����LRU_file_list
			temp->next = new blockList<fileInfo>;
			temp->next->element = iter;
			temp->next->next = NULL;

			total_file++;
		}
		else {
			blockList<fileInfo>* LRU_pre = LRU_file_list;
			blockList<fileInfo>* LRU_cur = LRU_file_list;
			while (LRU_cur->element->lock) {
				LRU_pre = LRU_cur;
				LRU_cur = LRU_cur->next;
				if (!LRU_cur) {
					//�����ļ���lock��
					//�׳��쳣
				}
			}
			iter = LRU_cur->element;
			close_file(iter); //�ر���Ҫ���滻���ļ�
			iter->file_name = file_name;
			iter->type = file_type;
			iter->first_block = NULL;

			//����LRU_file_list
			temp->next = LRU_cur;
			LRU_pre = LRU_cur->next;
			LRU_cur->next = NULL;
		}
	}
	else {
		//�ļ����ڴ�
		blockList<fileInfo>* LRU_pre = NULL;
		blockList<fileInfo>* LRU_cur = NULL;
		for (LRU_cur = LRU_file_list; LRU_cur; LRU_cur = LRU_cur->next) {
			if (LRU_cur->element->file_name == file_name)
				break;
			LRU_pre = LRU_cur;
		}
		if (LRU_cur != temp) {
			LRU_pre->next = LRU_cur->next;
			temp->next = LRU_cur;
			LRU_cur->next = NULL;
		}
	}

	return iter;
}

blockInfo* Buffer::get_file_block(string file_name, bool file_type, int block_num) {
	fileInfo* file_node = get_file_info(file_name, file_type);
	blockInfo* iter = NULL;
	blockInfo* block_pre = NULL;

	for (iter = file_node->first_block; iter; iter = iter->next) {
		if (iter->block_num == block_num) {
			break;
		}
		block_pre = iter;
	}

	blockList<blockInfo>* temp = NULL;
	for (temp = LRU_block_list; temp->next; temp = temp->next);

	if (!iter) { //��δ���ڴ�
		if (block_handle->next) {//�����������п�
			block_handle->next->block_reset(block_num, file_node);
			iter = block_handle->next;

			//�����������еĿ���ӵ�file_node��������
			if (!block_pre) {
				file_node->first_block = iter;
				block_handle->next = iter->next;
				file_node->first_block->next = NULL;
			}
			else {
				block_pre->next = iter;
				block_handle->next = iter->next;
				block_pre->next->next = NULL;
			}

			//����LRU_block_list
			temp->next = new blockList<blockInfo>;
			temp->next->element = iter;
			temp->next->next = NULL;

		}
		else {
			if (total_block < MAX_BLOCK) { //�����½���
				iter = new blockInfo(block_num, file_node);
				if (block_pre)
					block_pre->next = iter;
				else
					file_node->first_block = iter;
				//����LRU_block_list
				temp->next = new blockList<blockInfo>;
				temp->next->element = iter;
				temp->next->next = NULL;

				total_block++;
			}
			else { //LRU�滻
				//ȡ��LRU�����з�lock�Ŀ�
				blockList<blockInfo>* LRU_pre = LRU_block_list;
				blockList<blockInfo>* LRU_cur = LRU_block_list;
				while (LRU_cur->element->lock) {
					LRU_pre = LRU_cur;
					LRU_cur = LRU_cur->next;
					if (!LRU_cur) {
						//���п鶼lock��
						//�׳��쳣
					}
				}
				iter = LRU_cur->element;

				blockInfo* replace_block_pre = iter->file->first_block; //�޸ı��滻�ļ��е�����
				blockInfo* replace_block = NULL;
				for (replace_block = iter->file->first_block; replace_block; replace_block = replace_block->next) {
					if (replace_block == iter)
						break;
					replace_block_pre = replace_block;
				}
				if (replace_block_pre == replace_block)
					iter->file->first_block = replace_block->next;
				else
					replace_block_pre->next = replace_block->next;
				
				if (iter->dirty) { //�����ݻ�д
					write_back(iter->file->file_name, iter->file->type, iter);
				}

				iter->block_reset(block_num, file_node);
				if (block_pre)
					block_pre->next = iter;
				else
					file_node->first_block = iter;
				iter->next = NULL;
				//����LRU_block_list
				temp->next = LRU_cur;
				LRU_pre->next = LRU_cur->next;
				LRU_cur->next = NULL;
			}
		}
		load_data(file_name, file_type, iter);
	}
	else { //�����ڴ�
		//����LRU_block_list
		blockList<blockInfo>* LRU_pre = NULL;
		blockList<blockInfo>* LRU_cur = NULL;
		for (LRU_cur = LRU_block_list; LRU_cur; LRU_cur = LRU_cur->next) {
			if (LRU_cur->element->block_num == block_num)
				break;
			LRU_pre = LRU_cur;
		}
		if (LRU_cur != temp) {
			LRU_pre->next = LRU_cur->next;
			temp->next = LRU_cur;
			LRU_cur->next = NULL;
		}
	}
	
	//��ȡ�������ݵ�iter
	return iter;
}

void Buffer::load_data(string file_name, bool file_type, blockInfo* block_node) {
	fstream file;
	file.open(ROOT + (file_type ? "index/" : "record/") + file_name, ios::in|ios::out|ios::binary);
	if (!file.good())
		file.close();
	file.seekg(block_node->block_num * BLOCK_LEN, ios::beg);
	file.read(block_node->cBlock, BLOCK_LEN);
	
	file.close();
}

void Buffer::close_file(fileInfo* file_node) {
	for (blockInfo* iter = file_node->first_block; iter; iter = iter->next) {
		if (iter->dirty) { // �����ݿ��д
			write_back(file_node->file_name, file_node->type, iter);
		}
		//����LRU����
		blockList<blockInfo>* temp = NULL, *LRU_pre = LRU_block_list;
		for (temp = LRU_block_list; temp; temp = temp->next) {
			if (temp->element->block_num == iter->block_num && temp->element->file == iter->file)
				break;
			LRU_pre = temp;
		}
		LRU_pre->next = temp->next;
		delete temp;
	}
	blockInfo* iter = NULL;
	for (iter = block_handle; iter->next; iter = iter->next);
	iter->next = file_node->first_block;
}

void Buffer::write_back(string file_name, bool file_type, blockInfo* block_node) {
	fstream file;
	file.open(ROOT + (file_type ? "index/" : "record/") + file_name, ios::in | ios::out | ios::binary);

	file.seekp(block_node->block_num * BLOCK_LEN, ios::beg);
	file.write(block_node->cBlock, BLOCK_LEN);

	file.close();
}

void Buffer::set_dirty(blockInfo* block_node, bool dirty) {
	block_node->dirty = dirty;
}

void Buffer::set_lock(blockInfo* block_node, bool lock) {
	block_node->lock = lock;
}

void Buffer::set_lock(fileInfo* file_node, bool lock) {
	file_node->lock = lock;
}

char* Buffer::get_content(blockInfo* block_node) {
	return block_node->cBlock;
}

Buffer::~Buffer() {
	//�ر��ļ�, ����ļ�ͷ�б�
	for (fileInfo* iter = file_handle, *pre = NULL; iter;) {
		close_file(iter);
		pre = iter;
		iter = iter->next;
		delete pre;
	}
	//�����������
	for (blockInfo* iter = block_handle, *pre = NULL; iter;) {
		pre = iter;
		iter = iter->next;
		delete pre;
	}
	//���LRU_file_list
	for (blockList<fileInfo>* iter = LRU_file_list, *pre = NULL; iter;) {
		pre = iter;
		iter = iter->next;
		delete pre;
	}
	//���LRU_block_list
	for (blockList<blockInfo>* iter = LRU_block_list, *pre = NULL; iter;) {
		pre = iter;
		iter = iter->next;
		delete pre;
	}
}