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
	//输入：表名
	//输出：void
	//功能：建立表文件
	//异常：无异常处理（由catalog manager处理）
	void createTableFile(std::string tableName);
	//输入：表名
	//输出：void
	//功能：删除表文件
	//异常：无异常处理（由catalog manager处理）
	void dropTableFile(std::string table_name);
	//输入：表名，一个元组
	//输出：void
	//功能：向对应表中插入一条记录
	//异常：如果元组类型不匹配，抛出tuple_type_conflict异常。如果
	//主键冲突，抛出primary_key_conflict异常。如果unique属性冲突，
	//抛出unique_conflict异常。如果表不存在，抛出table_not_exist异常。
	void insertRecord(std::string tableName, Tuple &tuple, IndexManager* im, CatalogManager& cm);
	//输入：表名
	//输出：int(删除的记录数)
	//功能：删除对应表中所有记录（不删除表文件）
	//异常：如果表不存在，抛出table_not_exist异常
	int deleteRecord(std::string tableName, IndexManager* im, CatalogManager& cm);
	//输入：表名，目标属性，一个Where类型的对象
	//输出：int(删除的记录数)
	//功能：删除对应表中所有目标属性值满足Where条件的记录
	//异常：如果表不存在，抛出table_not_exist异常。如果属性不存在，抛出attribute_not_exist异常。
	//如果Where条件中的两个数据类型不匹配，抛出data_type_conflict异常。
	int deleteRecord(std::string tableName, std::string target_attr, Where where, IndexManager*im, CatalogManager& cm);
	//输入：表名
	//输出：Table类型对象
	//功能：返回整张表
	//异常：如果表不存在，抛出table_not_exist异常
	Table selectRecord(std::string tableName, CatalogManager& cm, std::string resultTableName = "tmptable");
	//输入：表名，目标属性，一个Where类型的对象
	//输出：Table类型对象
	//功能：返回包含所有目标属性满足Where条件的记录的表
	//异常：如果表不存在，抛出table_not_exist异常。如果属性不存在，抛出attribute_not_exist异常。
	//如果Where条件中的两个数据类型不匹配，抛出data_type_conflict异常。
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
