#include "Interpreter.h"

int main(){
	interpreter a;
	while (true) {
		a.getInstruction();
		a.executeInstruction();
		if (a.quit()) {
			break;
		}
	}

	return 0;
}
