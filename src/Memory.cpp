#include "Memory.h"
#include <cstring>
#include <sstream>
#include <string>

void Memory::load_file(std::istream &in) {
	std::string str;
	unsigned int current = 0;
	while (std::getline(in, str)) {
		if (str.empty()) continue;
		if (str.front() == '@') {
			current = atoi(str.data() + 1);
			continue;
		}
		std::istringstream is(str);
		is >> std::hex;
		unsigned int a = 0, b = 0, c = 0;
		while (is >> b) {
			a |= (b << ((c++) * 8));
			if (c == 4) {
				this->store_word(current, a);
				current += 4;
				a = c = 0;
			}
		}
	}
}
