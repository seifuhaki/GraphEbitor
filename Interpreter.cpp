#include "Interpreter.h"

Interpreter::Interpreter() {
	this->instruction.clear();
	this->instructionList.clear();
	this->ifQuit = false;
	this->errorOccur = false;
}

bool Interpreter::quit() {
	return this->ifQuit;
}

void Interpreter::getInstruction() {
	this->errorOccur = false;
	std::string currentLine;
	char buffer[256];
	this->instruction.clear();
	std::cout << "MiniSQL> ";
	while (true) {
		this->instruction += " ";
		std::cin.getline(buffer, 256);
		currentLine = buffer;
		this->instruction += currentLine;
		try {
			if (currentLine.find(';') != std::string::npos) {
				if (currentLine.at(currentLine.length() - 1) != ';') {
					throw syntaxError();
				}
				else {
					break;
				}
			}
		}
		catch (...) {
			std::cout << "Syntax Error: please input instructions ending with ';'." << std::endl;
			return;
		}
		std::cout << "      -> ";
	}
}

void Interpreter::executeInstruction() {
	// normalize
	this->errorOccur = false;
	this->instructionList.clear();
	this->instruction.erase(this->instruction.end() - 1);
	splitString(this->instruction, this->instructionList, " ");

	try {
		if (this->instructionList.size() == 0) {
			throw syntaxError();
		}
	}
	catch (...) {
		std::cout << "Syntax Error: empty input" << std::endl;
		return;
	}

	try {
		if (this->instructionList[0] == "quit" && this->instructionList.size() == 1) {
			std::cout << "Quitting MiniSQL..." << std::endl;
			throw quitMiniSQL();
		}
		else if (this->instructionList[0] == "create") {
			if (this->instructionList.size() > 1 && this->instructionList[1] == "table") {
				createTable();
			}
			else if (this->instructionList.size() > 1 && this->instructionList[1] == "index") {
				createIndex();
			}
			else {
				throw syntaxError();
			}
		}
		else if (this->instructionList[0] == "drop") {
			if (this->instructionList.size() > 1 && this->instructionList[1] == "table") {
				dropTable();
			}
			else if (this->instructionList.size() > 1 && this->instructionList[1] == "index") {
				dropIndex();
			}
			else {
				throw syntaxError();
			}
		}
		else if (this->instructionList[0] == "select") {
			if (this->instructionList.size() < 2 || this->instructionList[1] != "*") {
				throw syntaxError();
			}
			else if (this->instructionList.size() < 3 || this->instructionList[2] != "from") {
				throw syntaxError();
			}
			else {
				selectAllFrom();
			}
		}
		else if (this->instructionList[0] == "insert") {
			if (this->instructionList.size() < 2 || this->instructionList[1] != "into") {
				throw syntaxError();
			}
			else {
				insertInto();
			}
		}
		else if (this->instructionList[0] == "delete") {
			if (this->instructionList.size() < 2 || this->instructionList[1] != "from") {
				throw syntaxError();
			}
			else {
				deleteFrom();
			}
		}
		else if (this->instructionList[0] == "execfile") {
			execfile();
		}
		else {
			throw syntaxError();
		}
	}
	catch (syntaxError e) {
		std::cout << "Syntax Error: Invalid or unsupported syntax." << std::endl;
		this->errorOccur = true;
	}
	catch (quitMiniSQL e) {
		this->errorOccur = true;
		this->ifQuit = true;
	}
	catch (cannotOpenFile e) {
		this->errorOccur = true;
		std::cout << "File Open Error: Cannot open file '" << this->instructionList[1] << "'." << std::endl;
	}
	catch (...) {
		this->errorOccur = true;
		std::cout << "Undefined Error." << std::endl;
	}
}

void Interpreter::splitString(const std::string& s, std::vector<std::string>& v, const std::string& c) {
	std::string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (std::string::npos != pos2) {
		if (pos1 != pos2) {
			v.push_back(s.substr(pos1, pos2 - pos1));
		}
		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length()) {
		v.push_back(s.substr(pos1));
	}
}

void Interpreter::execfile() {
	char buffer[256];
	std::ifstream ifile;
	std::string currentLine;

	if (this->instructionList.size() != 2) {
		throw syntaxError();
	}

	ifile.open(this->instructionList[1]);
	
	if (!ifile.is_open()) {
		throw cannotOpenFile();
	}

	std::cout << "Executing file '" << this->instructionList[1] << "'..." << std::endl;

	this->instruction.clear();

	while (!ifile.eof()) {
		std::cout << "      -> ";
		ifile.getline(buffer, 256);
		currentLine = buffer;
		this->instruction += " ";
		this->instruction += currentLine;
		std::cout << currentLine << std::endl;
		try {
			if (currentLine.find(';') != std::string::npos) {
				if (currentLine.at(currentLine.length() - 1) != ';') {
					throw syntaxError();
				}
				else {
					executeInstruction();
					this->instruction.clear();
					if (this->errorOccur) {
						return;
					}
				}
			}
		}
		catch (syntaxError e) {
			std::cout << "Syntax Error: please input instructions ending with ';'." << std::endl;
			return;
		}
		catch (...) {
			std::cout << "Undefined Error." << std::endl;
			return;
		}
	}

	ifile.close();

}

void Interpreter::deleteFrom() {
	std::string tableName;
	std::string attributeName;
	std::string relation;
	std::string value;

	if (this->instructionList.size() < 3) {
		throw syntaxError();
	}

	tableName = this->instructionList[2];

	if (this->instructionList.size() == 3) {
		api.deleteRecord(tableName);
	}
	else if (this->instructionList.size() == 7) {
		if (this->instructionList[3] != "where") {
			throw syntaxError();
		}
		attributeName = this->instructionList[4];
		relation = this->instructionList[5];
		value = this->instructionList[6];	// int, float, char in string type, char[] are passed with ''
		if (relation != "=" && relation != "<>" && relation != "<" && relation != ">" && relation != "<=" && relation != ">=") {
			throw syntaxError();
		}
		api.deleteRecord(tableName, attributeName, relation, value);
	}
	else {
		throw syntaxError();
	}

}

void Interpreter::insertInto() {	// notice that there is no space in (...), values are supposed to be splitted with ','
	std::string tableName;
	std::vector<std::string> values;
	std::size_t last;

	if (this->instructionList.size() < 5) {
		throw syntaxError();
	}

	tableName = this->instructionList[2];
	last = this->instructionList.size() - 1;

	if (this->instructionList[3] != "values") {
		throw syntaxError();
	}

	if (this->instructionList[4].at(0) != '(' || this->instructionList[last].at(this->instructionList[last].length() - 1) != ')') {
		throw syntaxError();
	}

	this->instructionList[4].erase(this->instructionList[4].begin());
	this->instructionList[last].erase(this->instructionList[last].end() - 1);

	values.clear();

	for (int i = 4; i <= last; i++) {
		splitString(this->instructionList[i], values, ",");	// int, float, char in string type, char[] are passed with ''
	}

	api.insertRecord(tableName, values);
	
	// insert into ... values ... (API)
}

void Interpreter::selectAllFrom() {
	std::string tableName;
	std::vector<std::string> attributeNames;
	std::vector<std::string> relations;
	std::vector<std::string> values;
	std::size_t index;
	std::string relation;

	if (this->instructionList.size() < 4) {
		throw syntaxError();
	}

	tableName = this->instructionList[3];

	if (this->instructionList.size() == 4) {
		api.selectRecord(tableName);
		// select * from ... (API)
		return;
	}

	if (this->instructionList.size() < 5 || this->instructionList[4] != "where") {
		throw syntaxError();
	}

	index = 5;
	attributeNames.clear();
	relations.clear();
	values.clear();

	while (true) {
		if (this->instructionList.size() <= index) {
			throw syntaxError();
		}
		attributeNames.push_back(this->instructionList[index]);
		index++;

		if (this->instructionList.size() <= index) {
			throw syntaxError();
		}
		relation = this->instructionList[index];
		relations.push_back(this->instructionList[index]);
		if (relation != "=" && relation != "<>" && relation != "<" && relation != ">" && relation != "<=" && relation != ">=") {
			throw syntaxError();
		}
		index++;

		if (this->instructionList.size() <= index) {
			throw syntaxError();
		}
		values.push_back(this->instructionList[index]);
		index++;

		if (this->instructionList.size() == index) {
			break;
		}
		else if (this->instructionList[index] != "and") {
			throw syntaxError();
		}
		else {
			index++;
		}
	}
	api.selectRecord(tableName, attributeNames, relations, values);
	// select * from ... where ... and ... (API)
}

void Interpreter::createIndex() {
	std::string indexName;
	std::string tableName;
	std::string attributeName;
	std::size_t last;

	if (this->instructionList.size() < 6) {
		throw syntaxError();
	}

	indexName = this->instructionList[2];

	if (this->instructionList[3] != "on") {
		throw syntaxError();
	}

	tableName = this->instructionList[4];
	last = this->instructionList.size() - 1;

	if (this->instructionList[5].at(0) != '(' || this->instructionList[last].at(this->instructionList[last].length() - 1) != ')') {
		throw syntaxError();
	}

	this->instructionList[5].erase(this->instructionList[5].begin());
	this->instructionList[last].erase(this->instructionList[last].end() - 1);

	for (std::size_t i = last; i >= 5; i--) {
		if (this->instructionList[i].length() == 0) {
			this->instructionList.erase(this->instructionList.begin() + i);
		}
	}

	if (this->instructionList.size() != 6) {
		throw syntaxError();
	}

	attributeName = this->instructionList[5];

	api.createIndex(tableName,indexName,attributeName);
	// create index ... on ... (...) (API)

}

void Interpreter::dropTable() {
	std::string tableName;

	if (this->instructionList.size() < 3) {
		throw syntaxError();
	}

	tableName = this->instructionList[2];

	api.dropTable(tableName);
	// drop table ... (API)
}

void Interpreter::dropIndex() {
	std::string indexName;

	if (this->instructionList.size() < 3) {
		throw syntaxError();
	}

	indexName = this->instructionList[2];

	api.dropIndex(indexName);
	// drop index ... (API)

}

void Interpreter::createTable() {
	std::string tableName;
	std::size_t last;
	std::vector<std::string> attributeNames;
	std::vector<std::string> types;
	std::vector<bool> unique;	// true <=> unique, false <=> not unique
	std::size_t index;
	std::string primaryKey;
	std::string token;
	int num;

	if (this->instructionList.size() < 5) {
		throw syntaxError();
	}

	tableName = this->instructionList[2];
	last = this->instructionList.size() - 1;

	if (this->instructionList[3].at(0) != '(' || this->instructionList[last].at(this->instructionList[last].length() - 1) != ')') {
		throw syntaxError();
	}

	this->instructionList[3].erase(this->instructionList[3].begin());
	this->instructionList[last].erase(this->instructionList[last].end() - 1);

	for (std::size_t i = last; i >= 3; i--) {
		if (this->instructionList[i].length() == 0) {
			this->instructionList.erase(this->instructionList.begin() + i);
		}
	}

	index = 3;
	attributeNames.clear();
	types.clear();
	unique.clear();

	while (true) {
		if (this->instructionList.size() <= index) {
			throw syntaxError();
		}
		if (this->instructionList[index] == "primary") {
			if (!attributeNames.empty()) {
				break;
			}
			else {
				throw syntaxError();
			}
		}
		attributeNames.push_back(this->instructionList[index]);
		index++;

		if (this->instructionList.size() <= index) {
			throw syntaxError();
		}
		if (this->instructionList[index].find(',') != std::string::npos) {
			if (this->instructionList[index].at(this->instructionList[index].length() - 1) != ',') {
				throw syntaxError();
			}
			else {
				this->instructionList[index].erase(this->instructionList[index].end() - 1);
				token = this->instructionList[index];
				if (!(token.size() == 3 && token.substr(0, 3) == "int") && !(token.size() == 5 && token.substr(0, 5) == "float") && !(token.size() > 4 && token.substr(0, 4) == "char")) {
					throw syntaxError();
				}
				if (token.size() > 4 && token.substr(0, 4) == "char") {
					if (token.at(4) != '(' || token.at(token.length() - 1) != ')') {
						throw syntaxError();
					}
					token.erase(token.end() - 1);
					token.erase(token.begin() + 4);
					try {
						num = atoi(token.substr(4, token.size() - 4).c_str());
					}
					catch (...) {
						throw syntaxError();
					}
					if (num < 1 || num > 255) {
						throw syntaxError();
					}
				}
				types.push_back(token);
				unique.push_back(false);
				index++;
				continue;
			}
		}
		if (this->instructionList[index].find('(') != std::string::npos) {
			token = this->instructionList[index];
			if (!(token.size() == 3 && token.substr(0, 3) == "int") && !(token.size() == 5 && token.substr(0, 5) == "float") && !(token.size() > 4 && token.substr(0, 4) == "char")) {
				throw syntaxError();
			}
			if (token.size() > 4 && token.substr(0, 4) == "char") {
				if (token.at(4) != '(' || token.at(token.length() - 1) != ')') {
					throw syntaxError();
				}
				token.erase(token.end() - 1);
				token.erase(token.begin() + 4);
				try {
					num = atoi(token.substr(4, token.size() - 4).c_str());
				}
				catch (...) {
					throw syntaxError();
				}
				if (num < 1 || num > 255) {
					throw syntaxError();
				}
			}
			types.push_back(token);
			index++;
			if (this->instructionList.size() > index && this->instructionList[index] == ",") {
				index++;
				unique.push_back(false);
				continue;
			}
			if (this->instructionList.size() <= index || this->instructionList[index].length() < 6 || this->instructionList[index].substr(0, 6) != "unique") {
				throw syntaxError();
			}
			unique.push_back(true);
			if (this->instructionList[index].length() == 7 && this->instructionList[index].at(6) == ',') {
				index++;
				continue;
			}
			if (this->instructionList[index].length() > 7) {
				throw syntaxError();
			}
			index++;
			if (this->instructionList.size() <= index || this->instructionList[index] != ",") {
				throw syntaxError();
			}
			index++;
			continue;
		}
		else {
			throw syntaxError();
		}
	}

	index++;
	if (this->instructionList.size() <= index || this->instructionList[index] != "key") {
		throw syntaxError();
	}

	last = this->instructionList.size() - 1;
	index++;

	if (this->instructionList.size() <= index) {
		throw syntaxError();
	}

	if (this->instructionList[index].at(0) != '(' || this->instructionList[last].at(this->instructionList[last].length() - 1) != ')') {
		throw syntaxError();
	}

	this->instructionList[index].erase(this->instructionList[index].begin());
	this->instructionList[last].erase(this->instructionList[last].end() - 1);

	for (std::size_t i = last; i >= index; i--) {
		if (this->instructionList[i].length() == 0) {
			this->instructionList.erase(this->instructionList.begin() + i);
		}
	}

	if (this->instructionList.size() != index + 1) {
		throw syntaxError();
	}

	primaryKey = this->instructionList[index];

	TableInfo tableinfo;
	tableinfo.attributeNames = attributeNames;
	tableinfo.types = types;
	tableinfo.unique = unique;
	tableinfo.tableName = tableName;
	api.createTable(tableName,tableinfo, primaryKey);
	// types: int, float, char172, char99....
}


















