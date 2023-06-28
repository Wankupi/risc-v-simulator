#pragma once
#include <array>
#include <iostream>
#include <unordered_map>
#include <iomanip>
class Memory {
public:
	static constexpr int PageSize = 4096;
	Memory() = default;
	void load_file(std::istream &in);
	unsigned int load_word(unsigned int x) const {
		// if there is a load after a branch, it's address may be wrong.
		// so the check is necessary.
		if (pages.count(x / PageSize) == 0) return 0;
		return *reinterpret_cast<unsigned int const *>(pages.at(x / PageSize).data() + (x % PageSize));
	}
	unsigned short load_half(unsigned int x) const {
		if (pages.count(x / PageSize) == 0) return 0;
		return *reinterpret_cast<unsigned short const *>(pages.at(x / PageSize).data() + (x % PageSize));
	}
	unsigned char load_byte(unsigned int x) const {
		if (pages.count(x / PageSize) == 0) return 0;
		return *reinterpret_cast<unsigned char const *>(pages.at(x / PageSize).data() + (x % PageSize));
	}

	void store_word(unsigned int x, unsigned int val) {
		*reinterpret_cast<unsigned int *>(pages[x / PageSize].data() + (x % PageSize)) = val;
	}
	void store_half(unsigned int x, unsigned short val) {
		*reinterpret_cast<unsigned short *>(pages[x / PageSize].data() + (x % PageSize)) = val;
	}
	void store_byte(unsigned int x, unsigned char val) {
		*reinterpret_cast<unsigned char *>(pages[x / PageSize].data() + (x % PageSize)) = val;
	}

private:
	std::unordered_map<unsigned int, std::array<char, PageSize>> pages;
};
