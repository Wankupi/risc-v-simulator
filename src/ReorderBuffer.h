#pragma once
#include "LoopList.h"
#include "Register.h"
#include <array>
#include "bus.h"


struct ReorderBufferEntry {
	bool busy;
	bool ready;
	RoBType type;
	unsigned int res, res_addr;
	unsigned int instr_addr;
};

class ReorderBuffer {
public:
	void execute(RegisterUnit &regs) {
		if (list.empty()) return;
		auto &ele = list.arr[list.head];
		if (!ele.ready) return;
		list_next.pop();
		// TODO
		regs.set(ele.res_addr, ele.res);
	}

	void flush() {
		list_next = list;
	}

	bool full() { return list.full(); }
	void add(Instr2RoB const &entry);
	unsigned int add_index() const { return list.tail; }
public:
	LoopList<ReorderBufferEntry, 16> list;


private:
	LoopList<ReorderBufferEntry, 16> list_next;
};
