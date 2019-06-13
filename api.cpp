#include <algorithm>
#include <string>
#include <vector>
#include <iterator>
#include "api.h"

//构造函数
API::API() {}
//析构函数
API::~API() {}

Table API::selectRecord(std::string table_name, std::vector<std::string> target_attr, std::vector<Where> where, char operation)
{
	if (target_attr.size() == 0) {
		return record.selectRecord(table_name);
	}
	else if (target_attr.size() == 1) {
		return record.selectRecord(table_name, target_attr[0], where[0]);
	}
	else {
		Table table1 = record.selectRecord(table_name, target_attr[0], where[0]);
		Table table2 = record.selectRecord(table_name, target_attr[1], where[1]);

		if (operation)
			return joinTable(table1, table2, target_attr[0], where[0]);
		else
			return unionTable(table1, table2, target_attr[0], where[0]);
	}
}
int API::deleteRecord(std::string table_name, std::string target_attr, Where where)
{
	int result;
	if (target_attr == "")
		result = record.deleteRecord(table_name);

	else
		result = record.deleteRecord(table_name, target_attr, where);
	return result;
}
void API::insertRecord(std::string table_name, Tuple& tuple)
{
	record.insertRecord(table_name, tuple);
}
bool API::createTable(std::string tableName, TableInfo attribute, std::string primary)
{
	record.createTableFile(tableName);
	catalog.createTable(tableName, attribute.attributeNames, attribute.types, attribute.unique, primary);

	return true;
}
bool API::dropTable(std::string table_name)
{
	record.dropTableFile(table_name);
	catalog.dropTable(table_name);

	return true;
}
bool API::createIndex(std::string tableName, std::string index_name, std::string attrName)
{
	//构造所有的Index
	std::vector<std::string> table_name;
	std::vector<std::string> attributeNames;
	std::vector<std::string> types;
	std::vector<IndexInfo> indexinfo = catalog.getIndexInfo();
	for (int i = 0; i < indexinfo.size(); i++) {
		table_name[i] = indexinfo[i].tableName;
		attributeNames[i] = indexinfo[i].attributeName;
		types[i] = indexinfo[i].type;
	}
	IndexManager index(table_name, attributeNames, types);
	std::string file_path = "IndexManager\\" + tableName + "_" + attrName + ".txt";
	std::string type;

	catalog.createIndex(tableName, attrName, index_name);
	TableInfo attr = catalog.getTableInfo(tableName);
	for (int i = 0; i < attr.attributeNames.size(); i++) {
		if (attr.attributeNames[i] == attrName) {
			type = attr.types[i];
			break;
		}
	}
	index.createIndex(file_path, type);

	return true;
}
bool API::dropIndex(std::string tableName, std::string indexName, std::string attrName)
{
	//构造所有的Index
	std::vector<std::string> table_name;
	std::vector<std::string> attributeNames;
	std::vector<std::string> types;
	std::vector<IndexInfo> indexinfo = catalog.getIndexInfo();
	for (int i = 0; i < indexinfo.size(); i++) {
		table_name[i] = indexinfo[i].tableName;
		attributeNames[i] = indexinfo[i].attributeName;
		types[i] = indexinfo[i].type;
	}
	IndexManager index(table_name, attributeNames, types);

	std::string file_path = "IndexManager\\" + tableName + "_" + attrName + ".txt";
	std::string type;

	TableInfo attr = catalog.getTableInfo(tableName);
	for (int i = 0; i < attr.attributeNames.size(); i++) {
		if (attr.attributeNames[i] == attrName) {
			type = attr.types[i];
			break;
		}
	}
	index.dropIndex(file_path, type);
	catalog.dropIndex(indexName);

	file_path = "IndexManager\\" + tableName + "_" + attrName + ".txt";
	remove(file_path.c_str());
	return true;
}
//私有函数，用于多条件查询时的or条件合并
Table API::unionTable(Table &table1, Table &table2, std::string target_attr, Where where)
{
	Table result_table(table1);
	std::vector<Tuple>& result_tuple = result_table.getTuple();
	std::vector<Tuple> tuple1 = table1.getTuple();
	std::vector<Tuple> tuple2 = table2.getTuple();
	result_tuple = tuple1;

	int i;
	TableInfo attr = table1.getAttr();
	for (i = 0; i < 32; i++)
		if (attr.attributeNames[i] == target_attr)
			break;

	for (int j = 0; j < tuple2.size(); j++)
		if (!isSatisfied(tuple2[j], i, where))
			result_tuple.push_back(tuple2[j]);

	std::sort(result_tuple.begin(), result_tuple.end(), sortcmp);
	return result_table;

}
//私有函数，用于多条件查询时的and条件合并
Table API::joinTable(Table &table1, Table &table2, std::string target_attr, Where where)
{
	Table result_table(table1);
	std::vector<Tuple>& result_tuple = result_table.getTuple();
	std::vector<Tuple> tuple1 = table1.getTuple();
	std::vector<Tuple> tuple2 = table2.getTuple();

	int i;
	TableInfo attr = table1.getAttr();
	for (i = 0; i < 32; i++)
		if (attr.attributeNames[i] == target_attr)
			break;

	for (int j = 0; j < tuple2.size(); j++)
		if (isSatisfied(tuple2[j], i, where))
			result_tuple.push_back(tuple2[j]);

	std::sort(result_tuple.begin(), result_tuple.end(), sortcmp);
	return result_table;
}
//用于对vector的sort时排序
bool sortcmp(const Tuple &tuple1, const Tuple &tuple2)
{
	std::vector<data> data1 = tuple1.getData();
	std::vector<data> data2 = tuple2.getData();

	if(data1[0].type == "int") return data1[0].datai < data2[0].datai;
	else if(data1[0].type == "float")return data1[0].dataf < data2[0].dataf;
	else return data1[0].datas < data2[0].datas;
}
//用于对vector对合并时排序
bool calcmp(const Tuple &tuple1, const Tuple &tuple2)
{
	int i;

	std::vector<data> data1 = tuple1.getData();
	std::vector<data> data2 = tuple2.getData();

	for (i = 0; i < data1.size(); i++) {
		bool flag = false;
		if (data1[0].type == "int") {
			if (data1[0].datai != data2[0].datai)
				flag = true;
		}
		else if (data1[0].type == "float") {
			if (data1[0].dataf != data2[0].dataf)
				flag = true;
		}
		else{
			if (data1[0].datas != data2[0].datas)
				flag = true;
		}
		if (flag)
			break;
	}
	if (i == data1.size())
		return true;
	else
		return false;
}
bool isSatisfied(Tuple& tuple, int target_attr, Where where)
{
	std::vector<data> data = tuple.getData();

	switch (where.relation_character) {
	case LESS: {
		if(where.data.type == "int") return (data[target_attr].datai < where.data.datai);
		else if (where.data.type == "float") return (data[target_attr].dataf < where.data.dataf);
		else return (data[target_attr].datas < where.data.datas);
	} break;
	case LESS_OR_EQUAL: {
		if (where.data.type == "int") return (data[target_attr].datai < where.data.datai);
		else if (where.data.type == "float") return (data[target_attr].dataf < where.data.dataf);
		else return (data[target_attr].datas < where.data.datas);
	} break;
	case EQUAL: {
		if (where.data.type == "int") return (data[target_attr].datai < where.data.datai);
		else if (where.data.type == "float") return (data[target_attr].dataf < where.data.dataf);
		else return (data[target_attr].datas < where.data.datas);
	} break;
	case GREATER_OR_EQUAL: {
		if(where.data.type == "int") return (data[target_attr].datai < where.data.datai);
		else if (where.data.type == "float") return (data[target_attr].dataf < where.data.dataf);
		else return (data[target_attr].datas < where.data.datas);
	} break;
	case GREATER: {
		if (where.data.type == "int") return (data[target_attr].datai < where.data.datai);
		else if (where.data.type == "float") return (data[target_attr].dataf < where.data.dataf);
		else return (data[target_attr].datas < where.data.datas);
	} break;
	case NOT_EQUAL: {
		if (where.data.type == "int") return (data[target_attr].datai < where.data.datai);
		else if (where.data.type == "float") return (data[target_attr].dataf < where.data.dataf);
		else return (data[target_attr].datas < where.data.datas);
	} break;
	default:break;
	}
	return false;
}