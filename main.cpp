#include<iostream>
#include <string>
#include "Interpreter.h"

int main()
{
	Interpreter a;
	while (true) {
		a.getInstruction();
		a.executeInstruction();
		if (a.quit()) {
			break;
		}
	}
	system("pause");
}
