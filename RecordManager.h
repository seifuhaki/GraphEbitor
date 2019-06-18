#ifndef _RECORD_MANAGER_H_
#define _RECORD_MANAGER_H_ 1
#define INF 1000000
#define MAXFRAMESIZE 100
#define PAGESIZE 4096

#include <cstdio> 
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <sstream>
#include "IndexManager.h"
#include "CatalogManager.h"
#include "BufferManager.h"
#include "Template.h"
#include "Exception.h"
extern BufferManager bm;

class RecordManager {
public:
	void createTableFile(std::string tableName);
	void dropTableFile(std::string table_name);
	void insertRecord(std::string tableName, Tuple &tuple, IndexManager* im, CatalogManager& cm);
	int deleteRecord(std::string tableName, IndexManager* im, CatalogManager& cm);
	int deleteRecord(std::string tableName, std::string target_attr, Where where, IndexManager*im, CatalogManager& cm);
	Table selectRecord(std::string tableName, CatalogManager& cm, std::string resultTableName = "tmptable");
	Table selectRecord(std::string table_name, std::string target_attr, Where where, IndexManager* im, CatalogManager& cm, std::string result_table_name = "tmp_table_name");
	void createIndex(IndexManager* index_manager, std::string tableName, std::string target_attr, CatalogManager& cm);
private:
	//获取文件大小
	int getBlockNum(std::string table_name);
	//从内存中读取一个tuple
	Tuple readTuple(char* p, TableInfo attr);
	//获取一个tuple的长度
	int getTupleLength(char* p);
	//insertRecord的辅助函数
	void insertRecord1(char* p, int offset, int len, const std::vector<data>& v);
	//deleteRecord的辅助函数
	char* deleteRecord1(char* p);
	//判断插入的记录是否和其他记录冲突
	bool isConflict(std::vector<Tuple>& tuples, std::vector<data>& v, int index);
	//带索引查找
	std::vector<int> searchWithIndex(std::string tableName, std::string attributeName, Where where, IndexManager* im);
	//在块中进行条件删除
	int conditionDeleteInBlock(std::string table_name, int block_id, TableInfo attr, int index, Where where, IndexManager*im, CatalogManager& cm);
	//在块中进行条件查询
	void conditionSelectInBlock(std::string table_name, int block_id, TableInfo attr, int index, Where where, std::vector<Tuple>& v);

	BufferManager bm;
};
#endif
