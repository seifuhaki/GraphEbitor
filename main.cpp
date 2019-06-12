#include<iostream>
#include "RecordManager.h"

int main() {
	RecordManager rm;
	rm.createTableFile("my");
	int a;
	std::cin >> a;
	rm.dropTableFile("my");
	return 0;
}