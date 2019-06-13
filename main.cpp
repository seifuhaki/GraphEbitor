#include<iostream>
#include <string>
#include "api.h"

int main() {
	API api;
	std::string tableName = "my";
	Tuple tuple = Tuple();
	data data_in;
	data_in.type = "string";
	data_in.datas = "mysql";
	tuple.addData(data_in);
	createTable(std::string table_name, TableInfo attribute, std::string primary, IndexInfo index);
	insertRecord(std::string table_name, Tuple& tuple);
	int a;
	std::cin >> a;
	//rm.dropTableFile("my");
	return 0;
}