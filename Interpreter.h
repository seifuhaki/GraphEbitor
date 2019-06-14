#ifndef _INTERPRETER_H_
#define	_INTERPRETER_H_ 

#include "Exception.h"
#include "api.h"
#include <string>
#include <iostream>
#include <vector>
#include <fstream>


/* 需要以下函数 （数字为interpreter.cpp中行数）
	222 deleteAllRecord(std::string tableName)
	234 deleteRecord(std::string tableName, std::string attributeName, std::string relation, str::strinng value)
	272 insertRecord(std::string tableName, std::vector<std::string> values)
	292 selectAllRecord(std::string tableName)
	339 selectRecord(std::string tablName, std::vector<std::string> attributeNames, std::vector<std::string> relations, std::vector<std::string> values)
	382 createIndex(std::string tableName, std::string attributeName, atd::string, indexName)
	396 dropTable(std::string tableName)
	409 dropIndex(std::stirng IndexName)
	582 createTable(std::string tableName, std::vector<std::string> attributeNames, std::vector<std::string> types, std::vector<bool> unique, std::string primaryKey)
*/

class Interpreter {
private:
	std::string instruction;
	std::vector<std::string> instructionList;
	bool ifQuit;
	bool errorOccur;
	API api;

public:
	Interpreter();
	bool quit();
	void getInstruction();
	void executeInstruction();
	void splitString(const std::string& s, std::vector<std::string>& v, const std::string& c);
	void execfile();
	void deleteFrom();
	void insertInto();
	void selectAllFrom();
	void createIndex();
	void dropTable();
	void dropIndex();
	void createTable();
};

#endif
