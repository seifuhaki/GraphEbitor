#ifndef _BASIC_H_
#define _BASIC_H_ 1
#include <iostream>
#include <vector>
#include <string>
typedef enum {
	LESS,
	LESS_OR_EQUAL,
	EQUAL,
	GREATER_OR_EQUAL,
	GREATER,
	NOT_EQUAL
} WHERE;
struct data {
	std::string type;
	int datai;
	float dataf;
	std::string datas;
};
struct Where {
	data data; //数据
	WHERE relation_character;   //关系
};
class IndexInfo {
public:
	std::string indexName;
	std::string tableName;
	std::string attributeName;
};
class TableInfo {
public:
	std::string tableName;
	std::vector<std::string> attributeNames;
	std::vector<std::string> types;
	std::vector<bool> unique;
};
class Tuple {
private:
	std::vector<data> data_;  //存储元组里的每个数据的信息
	bool isDeleted_;
public:
	Tuple() : isDeleted_(false) {};
	Tuple(const Tuple &tuple_in);  //拷贝元组
	void addData(data data_in);  //新增元组
	std::vector<data> getData() const;  //返回数据
	int getSize();  //返回元组的数据数量
	bool isDeleted();
	void setDeleted();
	void showTuple();  //显示元组中的所有数据
};
class Table {
private:
	std::string tableName;  //表名
	std::vector<Tuple> tuple_;  //存放所有的元组
	std::vector<IndexInfo> index_;  //表的索引信息
public:
	TableInfo attr_;  //数据的类型
	//构造函数
	Table() {};
	Table(std::string title, TableInfo attr);
	Table(const Table &table_in);

	// int DataSize();  //每个tuple占的数据大小

	int setIndex(std::string attributeName, std::string index_name);  //插入索引，输入要建立索引的Attribute的编号，以及索引的名字，成功返回1失败返回0
	int dropIndex(std::string index_name);  //删除索引，输入建立的索引的名字，成功返回1失败返回0

	//private的输出接口
	std::string getTitle();
	TableInfo getAttr();
	std::vector<Tuple>& getTuple();
	std::vector<IndexInfo>& getIndex();

	void showTable(); //显示table的部分数据
	void showTable(int limit);
};
#endif