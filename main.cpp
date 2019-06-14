#include<iostream>
#include <string>
#include "api.h"

int main() {
	API api;
	std::string tableName = "my";
	std::string primary = "primary";
	std::string str = "ef";
	TableInfo tinfo;
	tinfo.tableName = tableName;
	tinfo.attributeNames.push_back(primary);
	tinfo.types.push_back("string");
	tinfo.unique.push_back(true);
	tinfo.attributeNames.push_back(str);
	tinfo.types.push_back("string");
	tinfo.unique.push_back(false);
	Table table = Table(tableName, tinfo);
	Tuple tuple =Tuple();
	data data_in;
	data_in.type = "string";
	data_in.datas = "mysql";
	tuple.addData(data_in);
	api.createTable(table.getTitle(), table.attr_, primary);
	//api.insertRecord(tableName, tuple);
	int a;
	std::cin >> a;
	return 0;
}