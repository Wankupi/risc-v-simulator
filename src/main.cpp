#include "Memory.h"
#include "cpu.h"
#include <fstream>
#include <iostream>

int main() {
	//	std::ifstream file("t.in", std::ios::in);
	Memory mem;
	mem.load_file(std::cin);
	CPU cpu;
	cpu.link_memory(&mem);
	try {
		int result = cpu.work();
		std::cout << result << std::endl;
	} catch (const char *str) {
		std::cerr << str << std::endl;
	}
	return 0;
}
