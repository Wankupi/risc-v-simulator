#pragma once
#include "Cache.h"
#include "Memory.h"
#include "bus.h"
#include <array>

struct LSB_entry : public Instr2LSB {
	bool busy;
};

class LoadStoreBuffer {
public:
	void execute(Cache &cache, Memory &mem, ResultType rs, RoB2LSB rob);
	void flush() {
		list = list_next;
		result = result_next;
	}
	void add(Instr2LSB const &entry);
	bool full();
	ResultType export_data() { return result; }
	void init() {
		on_clear();
	}
private:
	int next_issue_entry(ResultType lsb, ResultType rs, RoB2LSB rob);
	void update_dependencies(ResultType cr, ResultType rs, RoB2LSB rob);
	void on_clear();
private:
	std::array<LSB_entry, 16> list;
	ResultType result;

private:
	std::array<LSB_entry, 16> list_next;
	ResultType result_next;
};
