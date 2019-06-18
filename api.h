#ifndef _API_H_
#define _API_H_ 1

#include "basic.h"
#include "RecordManager.h"
#include "BufferManager.h"

//API接口。作为Interpreter层操作和底层Manager连接的接口
//包含程序的所有功能接口
//API只做初步的程序功能判断，具体的异常抛出仍由各底层Manager完成
class API {
public:
	//构造函数
	API();
	//析构函数
	~API();
	Table selectRecord(std::string table_name);
	Table selectRecord(std::string table_name, std::vector<std::string> target_attr, std::vector<std::string> relations, std::vector<std::string> values);
	int deleteRecord(std::string table_name);
	int deleteRecord(std::string table_name, std::string target_attr, std::string relation, std::string value);
	void insertRecord(std::string table_name, std::vector<std::string>values);
	bool createTable(std::string table_name, TableInfo attribute, std::string primary);
	bool dropTable(std::string table_name);
	bool createIndex(std::string table_name, std::string index_name, std::string attr_name);
	bool dropIndex(std::string indexName);
private:
	//私有函数，用于多条件查询时的and条件合并
	Table API::joinTable(std::string table_name, std::vector<Table> table, std::vector<std::string> target_attr, std::vector<Where> where);
private:
	RecordManager record;
	CatalogManager catalog;
	IndexManager *im;
};

//用于对vector的sort时排序
bool sortcmp(const Tuple &tuple1, const Tuple &tuple2);
//用于对vector对合并时对排序
bool calcmp(const Tuple &tuple1, const Tuple &tuple2);
bool isSatisfied(Tuple& tuple, int target_attr, Where where);
bool isConflict(std::vector<Tuple>& tuples, std::vector<data>& v, int index);
#endif
