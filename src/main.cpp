#include "Memory.h"
#include "cpu.h"
#include <iostream>

int main() {
	Memory mem;
	mem.load_file(std::cin);
	CPU cpu;
	cpu.link_memory(&mem);
	try {
		auto result = cpu.work();
		std::cout << result << std::endl;
	} catch (const char *str) {
		std::cout << str << std::endl;
		std::cerr << str << std::endl;
	}
	return 0;
}
