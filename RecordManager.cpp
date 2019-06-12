#include "RecordManager.h"

void RecordManager::createTableFile(std::string tableName) {
	tableName = tableName + ".txt";
	FILE* f = fopen(tableName.c_str(), "w");
	fclose(f);
}

void RecordManager::dropTableFile(std::string tableName) {
	tableName = tableName + ".txt";
	remove(tableName.c_str());
}

void RecordManager::insertRecord(std::string tableName, Tuple& tuple)
{
	std::string tmpName = tableName;
	tableName = tableName + ".txt";
	CatalogManager cm;
	//检测表是否存在
	if (!cm.hasTable(tmpName)){
		throw(tableNotExists());
	}
	std::vector<Data> v = tuple.getData();
	//这里我需要获取表的Attribute，来判断插入的Tuple是否合法
	//缺少一个getAttribute函数
	//你createTable的时候把Attribute全部存好

	//异常判断完成
	int blockNum = cm.getBlockNum(tableName);
	if (blockNum <= 0)blockNum = 1;
	char* p = bm.getPage(tableName, blockNum - 1);
	int i;
	for (i = 0; p[i] != '\0'&&i < PAGESIZE; i++);
	int j;
	int len = 0;
	//计算Tuple的长度
	for (j = 0; j < v.size(); j++) {
		Data d = v[j];
		switch (d.type) {
		case -1: {
			int t = getDataLength(d.datai);
			len += t; break;
		}
		case 0: {
			float t = getDataLength(d.dataf);
			len += t; break;
		}
		default: {
			len += d.datas.length();
		}
		}
	}
	len += v.size() + 7;
	int blockOffset;//最终记录所插入的块的编号
	//如果剩余的空间足够插入该tuple
	if (PAGESIZE - i >= len) {
		blockOffset = blockNum - 1;
		//插入该元组
		insertRecord1(p, i, len, v);
		//写回表文件
		int pageId = bm.getPageId(tableName, blockNum - 1);
		bm.modifyPage(pageId);
	}
	//如果剩余的空间不够
	else {
		blockOffset = blockNum;
		//新增一个块
		char* p = bm.getPage(tableName, blockNum);
		//在新增的块中插入该元组
		insertRecord1(p, 0, len, v);
		//写回表文件
		int pageId = bm.getPageId(tableName, blockNum);
		bm.modifyPage(pageId);
	}
	//更新索引，待添加

}

int RecordManager::deleteRecord(std::string tableName)
{
	std::string tmpName = tableName;
	tableName = tableName + ".txt";
	CatalogManager cm;
	//检测表是否存在
	if (!cm.hasTable(tmpName)) {
		throw tableNotExists();
	}
	
	int blockNum = cm.getBlockNum(tableName);
	//表文件大小为0时直接返回
	if(blockNum <= 0)
		return 0;
	//getAttribute
	//Indexmanager
	int count = 0;
	//遍历所有块
	for (int i = 0; i < blockNum; i++) {
		char *p = bm.getPage(tableName,i);
		char *t = p;
		while (*p != '\0'&&p < t + PAGESIZE) {
			//更新索引

			//删除记录
			p = deleteRecord1(p);
			count++;
		}
		int pageId = bm.getPageId(tableName, i);
		bm.modifyPage(pageId);
	}
	return count;
}

int RecordManager::deleteRecord(std::string tableName, std::string AttributeName, Where where) 
{
	std::string tmpName = tableName;
	tableName = tableName + ".txt";
	CatalogManager cm;
	if (!cm.hasTable(tmpName)) {
		throw tableNotExists();
	}
	//getAttribute
	int index = -1;
	bool flag = false;//判断是否存在索引

	int count = 0;
	if (flag == true && where.relation_character != NOT_EQUAL) {
		//有索引，通过索引获得块号
	}
	else {
		int blockNum = cm.getBlockNum(tableName);
		if (blockNum <= 0)return 0;
		for (int i = 0; i < blockNum; i++) {
			//count +=conditionDeleteInBlock();
		}
	}
	return count;
}

Table selectRecord(std::string tableName, std::string resultTableName = "tmptable")
{
	std::string tmpName = tableName;
	tableName = tableName + ".txt";
	CatalogManager cm;
	if (!cm.hasTable(tmpName)) {
		throw tableNotExists();
	}

	int blockNum = cm.getBlockNum(tableName);
	if(blockNum<=0)blockNum = 1;
	//getAttribute
	Table table(resultTableName, attr);
	std::vector<Tuple> &v = table.getTuple();
	//遍历所有块
	for (int i = 0; i < blockNum; i++) {
		char *p = bm.getPage(tableName, i);
		char *t = p;
		while (*p != '\0'&& p < t + PAGESIZE) {
			Tuple tuple = readTuple(p, attr);
			if (tuple.isDeleted() == false)
				v.push_back(tuple);
			int len = getTupleLength(p);
			p = p + len;
		}
	}
	return table;
}
//Insert的辅助函数
void RecordManager::insertRecord1(char* p, int offset, int len, const std::vector<Data>& v)
{
	std::stringstream stream;
	stream << len;
	std::string s = stream.str();
	while (s.length() < 4)
		s = "0" + s;
	for (int j = 0; j < s.length(); j++, offset++)
		p[offset] = s[j];
	for (int j = 0; j < v.size(); j++) {
		p[offset] = ' ';
		offset++;
		Data d = v[j];
		switch (d.type) {
		case -1: {
			copyString(p, offset, d.datai);
		}; break;
		case 0: {
			copyString(p, offset, d.dataf);
		}; break;
		default: {
			copyString(p, offset, d.datas);
		};
		}
	}
	p[offset] = ' ';
	p[offset + 1] = '0';
	p[offset + 2] = '\n';
}
//Delete的辅助函数
char* RecordManager::deleteRecord1(char* p) {
	int len = getTupleLength(p);
	p = p + len;
	*(p - 2) = '1';
	return p;
}
//从内存中读取一个tuple
Tuple RecordManager::readTuple(const char* p, Attribute attr) {
	Tuple tuple;
	p = p + 5;
	for (int i = 0; i < attr.num; i++) {
		Data data;
		data.type = attr.type[i];
		char tmp[100];
		int j;
		for (j = 0; *p != ' '; j++, p++) {
			tmp[j] = *p;
		}
		tmp[j] = '\0';
		p++;
		std::string s(tmp);
		switch (data.type) {
		case -1: {
			std::stringstream stream(s);
			stream >> data.datai;
		}; break;
		case 0: {
			std::stringstream stream(s);
			stream >> data.dataf;
		}; break;
		default: {
			data.datas = s;
		}
		}
		tuple.addData(data);
	}
	if (*p == '1')
		tuple.setDeleted();
	return tuple;
}
template <typename T>
int getDataLength(T data) {
	std::stringstream stream;
	stream << data;
	return stream.str().length();
}
template <typename T>
void copyString(char* p, int& offset, T data) {
	std::stringstream stream;
	stream << data;
	std::string s1 = stream.str();
	for (int i = 0; i < s1.length(); i++, offset++)
		p[offset] = s1[i];
}

