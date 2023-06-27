#pragma once
#include "LoopList.h"
#include "Register.h"
#include "bus.h"
#include <array>


struct ReorderBufferEntry : Instr2RoB {
	bool busy;
	bool ready;
};

class ReorderBuffer {
public:
	void execute(RegisterUnit &regs, ResultType rs, ResultType lsb);

	void flush() {
		list = list_next;
		newPC = newPC_next;
		clear_signal = clear_signal_next;
	}

	bool full() { return list.full(); }
	void add(Instr2RoB const &entry, RegisterUnit &regs);
	unsigned int add_index() const { return list.tail; }
	RoB2LSB data_to_LSB() const { return {clear_signal, !list.empty(), list.head}; }
	void init() { on_clear(); }

private:
	void on_clear();

public:
	LoopList<ReorderBufferEntry, 16> list;
	bool clear_signal;
	unsigned int newPC;

private:
	LoopList<ReorderBufferEntry, 16> list_next;
	bool clear_signal_next;
	unsigned int newPC_next;
};
