#ifndef _INTERPRETER_H_
#define	_INTERPRETER_H_ 

#include "Exception.h"
#include "api.h"
#include <string>
#include <iostream>
#include <vector>
#include <fstream>

class interpreter {
private:
	std::string instruction;
	std::vector<std::string> instructionList;
	bool ifQuit;
	bool errorOccur;
	// API api;

public:
	interpreter();
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