// 如何使用?
// 首先实例化一个BufferManager类，通过getPageId接口
// 获取块在内存中的页号。然后通过getPage接口得到对应文件的对应块
// 在内存中的句柄，调用getPage得到句柄之后即可读取或修改页的内容。
// 需要注意的是，如果修改了对应页的内容，需要调用
// modifyPage接口来标记该页已经被修改，否则可能修改会丢失。

// 总结:getPageId获得页号，调用getPage修改页，modifyPage记录修改 
#ifndef _BUFFER_MANAGER_H_
#define _BUFFER_MANAGER_H_ 1

#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#define MAXFRAMESIZE 100
#define PAGESIZE 4096

// Page类。磁盘文件中的每一块对应内存中的一个页（page)
class Page {
public:
	Page();
	void initialize();
	void setFileName(std::string file_name);
	std::string getFileName();
	void setBlockId(int block_id);
	int getBlockId();
	void setDirty(bool dirty);
	bool getDirty();
	void setRef(bool ref);
	bool getRef();
	void setAvaliable(bool avaliable);
	bool getAvaliable();
	char* getBuffer();
private:
	char buffer_[PAGESIZE];//每一页都是一个大小为PAGESIZE字节的数组
	std::string file_name_;//页所对应的文件名
	int block_id_;//页在所在文件中的块号(磁盘中通常叫块)
	bool dirty_;//dirty记录页是否被修改
	bool ref_;//ref变量用于时钟替换策略
	bool avaliable_;//avaliable标示页是否可以被使用(即将磁盘块load进该页)
};

// BufferManager类。对外提供操作缓冲区的接口。
class BufferManager {
public:
	BufferManager();
	BufferManager(int frame_size);
	~BufferManager();
	// 通过页号得到页的句柄(一个页的头地址)
	char* getPage(std::string file_name, int block_id);
	// 标记page_id所对应的页已经被修改
	void modifyPage(int page_id);
	// 将页写回文件
	int flushPage(int page_id, std::string file_name, int block_id);
	// 获取对应文件的对应块在内存中的页号，没有找到返回-1
	int getPageId(std::string file_name, int block_id);
private:
	Page* Frames;//缓冲池，实际上就是一个元素为Page的数组，实际内存空间将分配在堆上
	int frame_size_;//记录总页数
	int current_position_;//时钟替换策略需要用到的变量
	void initialize(int frame_size);//实际初始化函数
	// 获取一个闲置的页的页号(内部封装了时钟替换策略，但使用者不需要知道这些)
	int getEmptyPageId();
	// 讲对应文件的对应块载入对应内存页，对于文件不存在返回-1，否则返回0
	int loadDiskBlock(int page_id, std::string file_name, int block_id);
};

#endif