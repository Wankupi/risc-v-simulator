#pragma once
#include "Memory.h"
#include "bus.h"

class Cache {
public:
	void set(unsigned int addr_, unsigned int value_, unsigned int RoB_id_, LS_type type_) {
		if (stat != 0)
			throw "Cache is working!";
		stat_next = 2;
		data_ready_next = (stat_next == 0);
		addr_next = addr_;
		value_next = value_;
		RoB_id_next = RoB_id_;
		type_next = type_;
	}

	void execute(Memory &mem, bool clear_signal) {
		if (clear_signal) {
			on_clear();
			return;
		}
		if (stat > 0) {
			stat_next = stat - 1;
			data_ready_next = (stat == 1 && type != LS_type::hang);
		}
		else if (stat == 0) {
			if (type == LS_type::store_word)
				mem.store_word(addr, value);
			else if (type == LS_type::store_half)
				mem.store_half(addr, value);
			else if (type == LS_type::store_byte)
				mem.store_byte(addr, value);
		}
	}

	ResultType get_result(Memory &mem) {
		if (!data_ready)
			return {false, 0, 0};
		unsigned int val;
		if (type == LS_type::load_word)
			val = mem.load_word(addr);
		else if (type == LS_type::load_half)
			val = static_cast<signed short>(mem.load_half(addr));
		else if (type == LS_type::load_byte)
			val = static_cast<signed char>(mem.load_byte(addr));
		else if (type == LS_type::load_half_unsigned)
			val = mem.load_half(addr);
		else if (type == LS_type::load_byte_unsigned)
			val = mem.load_byte(addr);
		return {true, RoB_id, val};
	}

	void flush() {
		stat = stat_next;
		data_ready = data_ready_next;
		addr = addr_next;
		value = value_next;
		RoB_id = RoB_id_next;
		type = type_next;
	}
	bool ready() { return data_ready; }
	bool free() { return stat == 0; }

	void on_clear() {
		init();
	}

    void init() {
		stat_next = 0;
		data_ready_next = false;
		type_next = LS_type::hang;
		addr_next = value_next = RoB_id_next = 0;
	}
public:
	int stat;
	bool data_ready;
	LS_type type;
	unsigned int addr, value;
	unsigned int RoB_id;

private:
	int stat_next;
	bool data_ready_next;
	LS_type type_next;
	unsigned int addr_next, value_next;
	unsigned int RoB_id_next;
};