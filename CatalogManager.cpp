#include "CatalogManager.h"

CatalogManager::CatalogManager() {
	
}

void CatalogManager::createTable(const std::string tableName, const std::vector<std::string>& attributeNames, const std::vector<std::string>& types, const std::vector<bool>& unique, const std::string primaryKey) {
	if (hasTable(tableName)) {
		throw tableExists();
	}
	std::string info = "";
	info += addStr(tableName, 32);
	info += num2str(attributeNames.size(), 2);
	for (std::size_t i = 0; i < 16; i++) {
		if (i < attributeNames.size()) {
			info += addStr(types[i], 7);
			info += addStr(attributeNames[i], 32);
			if (unique[i]) {
				info += "1";
			}
			else {
				info += "0";
			}
		}
		else {
			for (int j = 0; j < 40; j++) {
				info += "#";
			}
		}
	}

	// 需要在API中先判断主键存在于所有字段名中
	for (std::size_t i = 0; i < attributeNames.size(); i++) {
		if (attributeNames[i] == primaryKey) {
			info += num2str(i, 2);
		}
	}

	// 没有索引
	info += "0";
	// 索引初始全用"#"填充
	for (int i = 1; i < 7 * 34; i++) {
		info += "#";
	}

	// 空余部分也用"#"填充
	for (std::string::size_type i = info.length(); i < 1024; i++) {
		info += "#";
	}

	int blockNum = getBlockNum(TableInfoPath);
	if (blockNum == 0) {
		blockNum = 1;
	}

	for (int i = 0; i < blockNum; i++) {
		char * buf = bm.getPage(TableInfoPath, i);
		int pageId = bm.getPageId(TableInfoPath, i);
		for (int j = 0; j < 3; j++) {
			if (buf[j*1024] == '#' || buf[j*1024] == '\0' || buf[j*1024] == ' ') {
				strncpy_s(buf+1024*j, PAGESIZE-1024*j, info.c_str(), 1024);
				bm.modifyPage(pageId);
				return;
			}
		}
	}
	char* buf = bm.getPage(TableInfoPath, blockNum);
	int pageId = bm.getPageId(TableInfoPath, blockNum);
	strncpy_s(buf, 4096, info.c_str(), 1024);
	bm.modifyPage(pageId);
	
}

bool CatalogManager::hasTable(const std::string tableName) {
	int blockNum = getBlockNum(TableInfoPath);
	if (blockNum == 0) {
		return false;
	}
	std::string temp = addStr(tableName, 32);
	for (int i = 0; i < blockNum; i++) {
		char * buf = bm.getPage(TableInfoPath, i);
		std::string check(buf);
		for (std::size_t j = 0; j < 3; j++) {
			if (check.size() < 1024*(j+1)) {
				continue;
			}
			if (temp == check.substr(1024*j, 32)) {
				return true;
			}
		}
	}
	return false;
}

int CatalogManager::getBlockNum(const std::string fileName) {
	char* p;
	int block_num = -1;
	do {
		p = bm.getPage(fileName, block_num + 1);
		block_num++;
	} while (p[0] != '\0');
	return block_num;
}

std::string CatalogManager::addStr(const std::string &str, std::size_t length) {
	if (str.length() > length) {
		throw nameTooLong();
	}
	std::string temp = "";
	temp += str;
	for (std::size_t i = temp.length(); i < length; i++) {
		temp += '#';
	}

	return temp;
}

std::string CatalogManager::num2str(int num, std::size_t length) {
	std::string str = "";
	if (num < 0) {
		throw negativeNum();
	}
	str += std::to_string(num);
	for (std::size_t i = str.length(); i < length; i++) {
		str += '#';
	}
		
	return str;
}

void CatalogManager::dropTable(const std::string tableName) {
	if (!hasTable(tableName)) {
		throw tableNotExists();
	}

	std::string temp = addStr(tableName, 32);
	int blockNum = getBlockNum(TableInfoPath);
	
	// 同时要删除indexInfo中对用索引
	for (int i = 0; i < blockNum; i++) {
		char * buf = bm.getPage(TableInfoPath, i);
		std::string check(buf);
		for (std::size_t j = 0; j < 3; j++) {
			if (check.size() < 1024*(j+1)) {
				continue;
			}
			if (temp == check.substr(1024 * j, 32)) {
				buf[1024 * j] = '#';
				std::string n = check.substr(1024 * j + 676, 1);
				int indexNum = atoi(n.c_str());
				for (int k = 0; k < indexNum; k++) {
					std::string idName = check.substr(1024 * j + 677 + 34 * k, 32);
					removeChara(idName, '#');
					dropIndex(idName);
				}
				int pageId = bm.getPageId(TableInfoPath, i);
				bm.modifyPage(pageId);
				return;
			}
		}
	}




}

void CatalogManager::createIndex(const std::string tableName, const std::string attributeName, const std::string indexName) {
	if (!hasTable(tableName)) {
		throw tableNotExists();
	}
	if (!hasAttribute(tableName, attributeName)) {
		throw attributeNotExists();
	}
	if (!isUnique(tableName, attributeName)) {
		throw attributeNotUnique();
	}
	if (attributeHasIndex(tableName, attributeName)) {
		throw duplicateIndexOnAttribute();
	}
	if (hasIndex(indexName)) {
		throw duplicateIndexName();
	}

	// TableInfo更改
	int blockNum = getBlockNum(TableInfoPath);
	std::string temp = addStr(tableName, 32);
	std::string tempa = addStr(attributeName, 32);
	for (int i = 0; i < blockNum; i++) {
		char * buf = bm.getPage(TableInfoPath, i);
		int pageId = bm.getPageId(TableInfoPath, i);
		std::string check(buf);
		for (std::size_t j = 0; j < 3; j++) {
			if (check.size() < 1024*(j+1)) {
				continue;
			}
			if (temp == check.substr(1024*j, 32)) {
				std::string t = check.substr(1024 * j+32, 2);
				removeChara(t, '#');
				int attributeNum = atoi(t.c_str());
				int id;
				for (int k = 0; k < attributeNum; k++) {
					if (check.substr(1024 * j + 34 + k * 40 + 7, 32) == tempa) {
						id = k;
						break;
					}
				}
				std::string n = check.substr(1024 * j + 676, 1);
				int indexNum = atoi(n.c_str());
				if (indexNum == maxIndexNum) {
					throw tooManyIndex();
				}
				else {
					std::string str = addStr(indexName, 32);
					std::string x = num2str(id, 2);
					for (int l = 0; l <= indexNum; l++) {
						if (buf[j * 1024 + 677 + 34 * l] == '#') {
							for (std::size_t k = 0; k < str.size(); k++) {
								buf[j * 1024 + 677 + 34 * l + k] = str.at(k);
							}
							for (std::size_t k = 0; k < 2; k++) {
								buf[j * 1024 + 677 + 34 * l + 32 + k] = x.at(k);
							}
							break;
						}
					}
					
					buf[j * 1024 + 676] += 1; // 因为写的时候最大是7，所以直接加1，如果调整最大值这边也要调整
				}
				bm.modifyPage(pageId);
				break;
			}
		}
	}

	// indexInfo更改
	blockNum = getBlockNum(IndexInfoPath);
	for (int i = 0; i < blockNum; i++) {
		char * buf = bm.getPage(IndexInfoPath, i);
		int pageId = bm.getPageId(IndexInfoPath, i);
		for (int j = 0; j < PAGESIZE / 96 - 1; j++) {
			if (buf[j * 96] == '#' || buf[j * 96] == '\0' || buf[j * 96] == -1) {
				std::string info = "";
				info += addStr(tableName, 32);
				info += addStr(attributeName, 32);
				info += addStr(indexName, 32);
				strncpy_s(buf + 96 * j, PAGESIZE- 96 * j, info.c_str(), 96);
				bm.modifyPage(pageId);
				return;
			}
		}
	}

	char * buf = bm.getPage(IndexInfoPath, blockNum);
	int pageId = bm.getPageId(IndexInfoPath, blockNum);
	std::string info = "";
	info += addStr(tableName, 32);
	info += addStr(attributeName, 32);
	info += addStr(indexName, 32);
	strncpy_s(buf, PAGESIZE, info.c_str(), 96);
	bm.modifyPage(pageId);

}

bool CatalogManager::hasAttribute(const std::string tableName, const std::string attributeName) {
	int blockNum = getBlockNum(TableInfoPath);
	std::string temp = addStr(tableName, 32);

	for (int i = 0; i < blockNum; i++) {
		char * buf = bm.getPage(TableInfoPath, i);
		std::string check(buf);
		for (std::size_t j = 0; j < 3; j++) {
			if (check.size() < 1024*(j+1)) {
				continue;
			}
			if (temp == check.substr(1024*j, 32)) {
				std::string n = check.substr(1024 * j + 32, 2);
				removeChara(n, '#');
				int attributeNum = atoi(n.c_str());
				std::string t = addStr(attributeName, 32);
				for (int k = 0; k < attributeNum; k++) {
					if (check.substr(1024 * j + 34 + k * 40 + 7, 32) == t) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

void CatalogManager::removeChara(std::string &str, char c) {
	for (std::size_t i = str.size() - 1; i >= 0 && i < str.size(); i-- ) {
		if (str.at(i) == c) {
			str.erase(str.begin() + i);
		}
	}
}

bool CatalogManager::attributeHasIndex(const std::string tableName, const std::string attributeName) {
	int blockNum = getBlockNum(IndexInfoPath);
	std::string temp = addStr(tableName, 32);

	for (int i = 0; i < blockNum; i++) {
		char * buf = bm.getPage(IndexInfoPath, i);
		std::string check(buf);
		for (std::size_t j = 0; j < PAGESIZE/96 - 1; j++) {
			if (check.size() < 96*(j+1)) {
				continue;
			}
			if (temp == check.substr(96*j, 32)) {
				std::string n = check.substr(96 * j + 32, 32);
				std::string t = addStr(attributeName, 32);
				if (n == t) {
					return true;
				}
			}
		}
	}

	return false;
}

bool CatalogManager::hasIndex(const std::string indexName){
	int blockNum = getBlockNum(IndexInfoPath);
	std::string temp = addStr(indexName, 32);
	for (int i = 0; i < blockNum; i++) {
		char * buf = bm.getPage(IndexInfoPath, i);
		std::string check(buf);
		for (int j = 0; j < PAGESIZE / 96 - 1; j++) {
			if (check.size() < 96*(j+1)) {
				continue;
			}
			if (temp == check.substr(96*j+64, 32)) {
				return true;
			}
		}
	}
	return false;

}

void CatalogManager::dropIndex(const std::string indexName) {
	if (!hasIndex(indexName)) {
		throw indexNotExist();
	}
	std::string tableName;
	int blockNum = getBlockNum(IndexInfoPath);
	std::string temp = addStr(indexName, 32);
	// indexInfo
	for (int i = 0; i < blockNum; i++) {
		char * buf = bm.getPage(IndexInfoPath, i);
		int pageId = bm.getPageId(IndexInfoPath, i);
		std::string check(buf);
		for (int j = 0; j < PAGESIZE / 96 - 1; j++) {
			if (check.size() < 96*(j+1)) {
				continue;
			}
			if (temp == check.substr(96*j+64, 32)) {
				tableName = check.substr(96 * j, 32);
				std::string t = addStr("", 96);
				strncpy_s(buf + 96 * j, PAGESIZE-96*j, t.c_str(), 96);
				bm.modifyPage(pageId);
				break;
			}
		}
	}
	// tableInfo
	blockNum = getBlockNum(TableInfoPath);
	for (int i = 0; i < blockNum; i++) {
		char * buf = bm.getPage(TableInfoPath, i);
		int pageId = bm.getPageId(TableInfoPath, i);
		std::string check(buf);
		for (std::size_t j = 0; j < 3; j++) {
			if (check.size() < 1024*(j+1)) {
				continue;
			}
			if (tableName == check.substr(1024*j, 32)) {
				std::string n = check.substr(1024 * j+676, 1);
				int indexNum = atoi(n.c_str());
				for (int k = 0; k < indexNum; k++) {
					if (temp == check.substr(1024 * j+677 + 34 * k, 32)) {
						std::string t = addStr("#", 34);
						for (std::size_t l = 0; l < 34; l++) {
							buf[j * 1024 + 677 + 34 * k + l] = t.at(l);
						}
						break;
					}
				}

				buf[j * 1024 + 676] -= 1;
				bm.modifyPage(pageId);
				break;
			}
		}
	}



}

bool CatalogManager::isUnique(const std::string tableName, const std::string attributeName) {
	int blockNum = getBlockNum(TableInfoPath);
	std::string temp = addStr(tableName, 32);

	for (int i = 0; i < blockNum; i++) {
		char * buf = bm.getPage(TableInfoPath, i);
		std::string check(buf);
		for (std::size_t j = 0; j < 3; j++) {
			if (check.size() < 1024 * (j + 1)) {
				continue;
			}
			if (temp == check.substr(1024 * j, 32)) {
				std::string n = check.substr(1024 * j + 32, 2);
				removeChara(n, '#');
				int attributeNum = atoi(n.c_str());
				std::string t = addStr(attributeName, 32);
				for (int k = 0; k < attributeNum; k++) {
					if (check.substr(1024 * j + 34 + k * 40 + 7, 32) == t) {
						if (check.substr(1024 * j + 34 + k * 40 + 39, 1) == "1") {
							return true;
						}
						else {
							return false;
						}
					}
				}
			}
		}
	}
	return false;
}

std::vector<IndexInfo> CatalogManager::getIndexInfo() {

	std::vector<IndexInfo> results;
	results.clear();
	int blockNum = getBlockNum(IndexInfoPath);
	for (int i = 0; i < blockNum; i++) {
		char * buf = bm.getPage(IndexInfoPath, i);
		std::string check(buf);
		for (int j = 0; j < PAGESIZE / 96 - 1; j++) {
			if (check.size() < 96 * (j + 1)) {
				continue;
			}
			if (check.substr(96 * j, 1) != "#") {
				IndexInfo newIndex;
				std::string tn = check.substr(96 * j, 32);
				std::string an = check.substr(96 * j + 32, 32);
				std::string in = check.substr(96 * j + 64, 32);

				removeChara(tn, '#');
				removeChara(an, '#');
				removeChara(in, '#');

				newIndex.attributeName = an;;
				newIndex.indexName = in;
				newIndex.tableName = tn;
				newIndex.type = getType(tn, an);

				results.push_back(newIndex);
			}
		}
	}
	return results;
}

TableInfo CatalogManager::getTableInfo(const std::string tableName) {
	if (!hasTable(tableName)) {
		throw tableNotExists();
	}

	int blockNum = getBlockNum(TableInfoPath);
	std::string temp = addStr(tableName, 32);
	TableInfo result;
	result.attributeNames.clear();
	result.types.clear();
	result.unique.clear();
	result.tableName = tableName;
	for (int i = 0; i < blockNum; i++) {
		char * buf = bm.getPage(TableInfoPath, i);
		std::string check(buf);
		for (std::size_t j = 0; j < 3; j++) {
			if (check.size() < 1024 * (j + 1)) {
				continue;
			}
			if (temp == check.substr(1024 * j, 32)) {
				std::string n = check.substr(1024 * j + 32, 2);
				removeChara(n, '#');
				int attributeNum = atoi(n.c_str());
				for (int k = 0; k < attributeNum; k++) {
					std::string an = check.substr(1024 * j + 34 + k * 40 + 7, 32);
					std::string ty = check.substr(1024 * j + 34 + k * 40, 7);
					bool uq;
					if (check.substr(1024 * j + 34 + k * 40 + 39, 1) == "1") {
						uq = true;
					}
					else {
						uq = false;
					}
					removeChara(an, '#');
					removeChara(ty, '#');
					result.attributeNames.push_back(an);
					result.types.push_back(ty);
					result.unique.push_back(uq);
				}
			}
		}
	}
	return result;
}

std::string CatalogManager::getType(const std::string tableName, const std::string attributeName) {

	std::string type;
	int blockNum = getBlockNum(TableInfoPath);
	std::string temp = addStr(tableName, 32);

	for (int i = 0; i < blockNum; i++) {
		char * buf = bm.getPage(TableInfoPath, i);
		std::string check(buf);
		for (std::size_t j = 0; j < 3; j++) {
			if (check.size() < 1024 * (j + 1)) {
				continue;
			}
			if (temp == check.substr(1024 * j, 32)) {
				std::string n = check.substr(1024 * j + 32, 2);
				removeChara(n, '#');
				int attributeNum = atoi(n.c_str());
				std::string t = addStr(attributeName, 32);
				for (int k = 0; k < attributeNum; k++) {
					if (check.substr(1024 * j + 34 + k * 40 + 7, 32) == t) {
						type = check.substr(1024 * j + 34 + k * 40, 7);
						removeChara(type, '#');
					}
				}
			}
		}
	}
	return type;
}




