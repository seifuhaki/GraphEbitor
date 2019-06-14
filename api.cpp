#include <algorithm>
#include <string>
#include <vector>
#include <iterator>
#include "api.h"

//构造函数
API::API() {}
//析构函数
API::~API() {}
Table API::selectRecord(std::string table_name)
{
	return record.selectRecord(table_name);
}
//need type
Table API::selectRecord(std::string table_name, std::vector<std::string> target_attr, std::vector<std::string> relations, std::vector<std::string> values)
{
	std::vector<Where> where;
	if (relations[0] == "=")where[0].relation_character = EQUAL;
	else if (relations[0] == "<>")where[0].relation_character = NOT_EQUAL;
	else if (relations[0] == "<")where[0].relation_character = LESS;
	else if (relations[0] == ">")where[0].relation_character = GREATER;
	else if (relations[0] == "<=")where[0].relation_character = LESS_OR_EQUAL;
	else if (relations[0] == ">=")where[0].relation_character = GREATER_OR_EQUAL;
	std::string type = catalog.getType(table_name, target_attr[0]);
	if (type == "int") { where[0].data.type = "int"; where[0].data.datai = std::stol(values[0]); }
	else if (type == "float") { where[0].data.type = "float"; where[0].data.dataf = std::stof(values[0]); }
	else { where[0].data.type = "string"; where[0].data.datas = values[0]; }
	Table table;
	table = record.selectRecord(table_name, target_attr[0], where[0]);
	for (int i = 1; i < values.size(); i++) {
		if (relations[i] == "=")where[i].relation_character = EQUAL;
		else if (relations[i] == "<>")where[i].relation_character = NOT_EQUAL;
		else if (relations[i] == "<")where[i].relation_character = LESS;
		else if (relations[i] == ">")where[i].relation_character = GREATER;
		else if (relations[i] == "<=")where[i].relation_character = LESS_OR_EQUAL;
		else if (relations[i] == ">=")where[i].relation_character = GREATER_OR_EQUAL;
		std::string type = catalog.getType(table_name, target_attr[0]);
		if (type == "int") { where[i].data.type = "int"; where[i].data.datai = std::stol(values[i]); }
		else if (type == "float") { where[i].data.type = "float"; where[i].data.dataf = std::stof(values[i]); }
		else { where[0].data.type = "string"; where[i].data.datas = values[i]; }
		Table table_ = record.selectRecord(table_name, target_attr[i], where[i]);
		table = joinTable(table, table_, target_attr[0], where[0]);
	}
	return table;
}
int API::deleteRecord(std::string table_name)
{
	int result;
	result = record.deleteRecord(table_name);
	return result;
}
int API::deleteRecord(std::string table_name, std::string target_attr, std::string relation,std::string value)
{
	Where where;
	if(relation == "=")where.relation_character = EQUAL;
	else if (relation == "<>")where.relation_character = NOT_EQUAL;
	else if (relation == "<")where.relation_character = LESS;
	else if (relation == ">")where.relation_character = GREATER;
	else if (relation == "<=")where.relation_character = LESS_OR_EQUAL;
	else if (relation == ">=")where.relation_character = GREATER_OR_EQUAL;
	std::string type = catalog.getType(table_name, target_attr);
	if (type == "int") { where.data.type = "int"; where.data.datai = std::stol(value); }
	else if(type == "float") { where.data.type = "float"; where.data.dataf = std::stof(value); }
	else { where.data.type = "string"; where.data.datas = value; }
	int result;
	result = record.deleteRecord(table_name, target_attr, where);
	return result;
}
//need type
void API::insertRecord(std::string table_name, std::vector<std::string>values)
{
	Tuple tuple = Tuple();
	TableInfo tableinfo = catalog.getTableInfo(table_name);
	for (int i = 0; i < values.size(); i++) {
		std::string type = catalog.getType(table_name, tableinfo.attributeNames[i]);
		data data_in;
		if (type == "int") { data_in.type = "int"; data_in.datai = std::stol(values[i]); }
		else if (type == "float") { data_in.type = "float"; data_in.dataf = std::stof(values[i]); }
		else { data_in.type = "string"; data_in.datas = values[i]; }
		tuple.addData(data_in);
	}
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
bool API::dropIndex(std::string indexName)
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
	IndexInfo indexinfo_ = catalog.getIndexInfo(indexName);
	std::string file_path = "IndexManager\\" + indexinfo_.tableName + "_" + indexinfo_.attributeName + ".txt";
	std::string type;

	TableInfo attr = catalog.getTableInfo(indexinfo_.tableName);
	for (int i = 0; i < attr.attributeNames.size(); i++) {
		if (attr.attributeNames[i] == indexinfo_.attributeName) {
			type = attr.types[i];
			break;
		}
	}
	index.dropIndex(file_path, type);
	catalog.dropIndex(indexName);

	file_path = "IndexManager\\" + indexinfo_.tableName + "_" + indexinfo_.attributeName + ".txt";
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