#pragma once
#include "Cache.h"
#include "LoopList.h"
#include "Memory.h"
#include "bus.h"
#include <array>

struct LSB_entry : public Instr2LSB {
	bool busy;
};

class LoadStoreBuffer {
public:
	void execute(ResultType rs, RoB2LSB rob);
	void flush() {
		list = list_next;
	}
	void add(Instr2LSB const &entry);
	bool full();
	ResultType export_data() { return cache->get_result(); }
	void init(Cache *pCache) {
		on_clear();
		cache = pCache;
	}

private:
	int next_issue_entry(ResultType lsb, ResultType rs, RoB2LSB rob);
	void update_dependencies(ResultType cr, ResultType rs, RoB2LSB rob);
	void on_clear();

public:
	Cache *cache;

private:
	LoopList<LSB_entry, 16> list;

private:
	LoopList<LSB_entry, 16> list_next;
};
