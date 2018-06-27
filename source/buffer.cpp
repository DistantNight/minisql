#include "buffer.h"
#include <fstream>
#include <cstdlib>
#include <stdexcept>

using std::fstream;
using std::ios;
const string ROOT = "./data/";

void blockInfo::block_reset(int32_t block_num, fileInfo* file)
{
	this->block_num = block_num;
	memset(this->cBlock, 0, BLOCK_LEN);
	this->dirty = false;
	this->lock = false;
	this->file = file;
}

fileInfo* Buffer::get_file_info(const string& file_name, bool file_type)
{
	fileInfo* iter = nullptr;

	for (auto& temp : file_handle)
	{
		if (temp->file_name == file_name)
		{
			iter = temp;
			break;
		}
	}

	if (!iter)
	{
		//文件未打开
		if (total_file < MAX_FILE_ACTIVE)
		{
			// 可以打开文件
			iter = new fileInfo(file_name, file_type);
			file_handle.push_back(iter);

			LRU_file_list.push_back(iter);
			total_file++;
		}
		else
		{
			fileInfo* LRU_cur = nullptr;

			for (auto &temp: LRU_file_list)
			{
				if (! temp->lock)
				{
					LRU_cur = temp;
					break;
				}
			}

			if (!LRU_cur)
			{
				throw std::logic_error("all active files are locked, cannot open a new file");
			}
			else {
				close_file(LRU_cur); //关闭需要被替换的文件
			}

			LRU_cur->file_name = file_name;
			LRU_cur->type = file_type;

			//更新LRU_file_list

			LRU_file_list.remove(LRU_cur);
			LRU_file_list.push_back(LRU_cur);

			iter = LRU_cur;
		}
	}
	else
	{
		//文件在内存

		fileInfo* LRU_cur = nullptr;

		for (auto &temp : LRU_file_list)
		{
			if (temp->file_name == file_name)
			{
				LRU_cur = temp;
				break;
			}
		}

		LRU_file_list.remove(LRU_cur);
		LRU_file_list.push_back(LRU_cur);

		iter = LRU_cur;
	}

	return iter;
}

blockInfo* Buffer::get_file_block(const string& file_name, bool file_type, int block_num)
{
	fileInfo* file_node = get_file_info(file_name, file_type);
	blockInfo* iter = nullptr;

	for (auto &temp:file_node->block_list)
	{
		if (temp->block_num == block_num)
		{
			iter = temp;
			break;
		}
	}

	if (!iter)
	{
		//块未在内存
		if (!block_handle.empty())
		{
			iter = block_handle.back();
			iter->block_reset(block_num, file_node);
			file_node->block_list.push_back(iter);
            block_handle.remove(iter);
			LRU_block_list.push_back(iter);
		}
		else
		{
			if (total_block < MAX_BLOCK)
			{
				//可以新建块

				iter = new blockInfo(block_num, file_node);
                file_node->block_list.push_back(iter);
				//更新LRU_block_list
				LRU_block_list.push_back(iter);

				total_block++;
			}
			else
			{
				//LRU替换
				//取出LRU链表中非lock的块

				blockInfo* LRU_cur = nullptr;

				for (auto &temp: LRU_block_list)
				{
					if (! temp->lock)
					{
						LRU_cur = temp;
						break;
					}
				}

				if (! LRU_cur)
				{
					throw std::logic_error("all blocks in the memory are locked, cannot get a new block");
				}
				else
				{
					iter = LRU_cur;

					if (iter->dirty)
					{
						write_back(iter->file->file_name, iter->file->type, iter);
					}
					
					iter->file->block_list.remove(iter);

					iter->block_reset(block_num, file_node);

					file_node->block_list.push_back(iter);

					//更新LRU_block_list
					LRU_block_list.remove(iter);
					LRU_block_list.push_back(iter);
				}
			}
		}
		if (iter)
			load_data(file_name, file_type, iter);
	}
	else
	{
		//块在内存
		//更新LRU_block_list
		LRU_block_list.remove(iter);
		LRU_block_list.push_back(iter);
	}

	//读取磁盘数据到iter
	return iter;
}

void Buffer::load_data(const string& file_name, bool file_type, blockInfo* block_node)
{
	fstream file;
	if (block_node->block_num == 0)
		file.open(ROOT + (file_type ? "index/" : "record/") + file_name, ios::in | ios::out | ios::binary | ios::app);
	else
		file.open(ROOT + (file_type ? "index/" : "record/") + file_name, ios::in | ios::out | ios::binary);
	if (file.is_open())
	{
		file.seekg(block_node->block_num * BLOCK_LEN, ios::beg);
		file.read(block_node->cBlock, BLOCK_LEN);
		file.close();
	}
	else
	{
		load_data(file_name, file_type, block_node);
	}
}

void Buffer::close_file(fileInfo* file_node)
{
	for (auto &iter : file_node->block_list)
	{
		if (iter->dirty)
		{
			write_back(file_node->file_name, file_node->type, iter);
		}
	}

	for (auto &iter : file_node->block_list)
	{
		LRU_block_list.remove(iter);
		block_handle.push_back(iter);
	}

	file_node->block_list.clear();
}

void Buffer::write_back(const string& file_name, bool file_type, blockInfo* block_node)
{
	fstream file;
	file.open(ROOT + (file_type ? "index/" : "record/") + file_name, ios::in | ios::out | ios::binary);
	if (file.is_open())
	{
		file.seekp(block_node->block_num * BLOCK_LEN, ios::beg);
		file.write(block_node->cBlock, BLOCK_LEN);
		file.close();
	}
	else
	{
		write_back(file_name, file_type, block_node);
	}
}

void Buffer::set_dirty(blockInfo* block_node, bool dirty)
{
	block_node->dirty = dirty;
}

void Buffer::set_lock(blockInfo* block_node, bool lock)
{
	block_node->lock = lock;
}

void Buffer::set_lock(fileInfo* file_node, bool lock)
{
	file_node->lock = lock;
}

char* Buffer::get_content(blockInfo* block_node)
{
	return block_node->cBlock;
}

void Buffer::remove_file(const string& file_name, bool file_type)
{
	fileInfo* file_node = get_file_info(file_name, file_type);
	close_file(file_node);
	remove((ROOT + (file_type ? "index/" : "record/") + file_name).c_str());
}

Buffer::~Buffer()
{
	for (auto &temp : file_handle)
	{
		close_file(temp);
        delete temp;
	}

    for (auto &temp:block_handle)
    {
        delete temp;
    }
}