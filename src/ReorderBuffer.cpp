#include "ReorderBuffer.h"

void ReorderBuffer::execute(RegisterUnit &regs, ResultType rs, ResultType lsb) {
	if (clear_signal) {
		on_clear();
		return;
	}
	clear_signal_next = false;
	if (rs.ready) {
		list_next[rs.RoB_id].value = rs.value;
		list_next[rs.RoB_id].ready = true;
	}
	if (lsb.ready) {
		list_next[lsb.RoB_id].value = lsb.value;
		list_next[lsb.RoB_id].ready = true;
	}
	if (list.empty()) return;
	unsigned int id = list.head;
	auto const &entry = list[id];
	if (entry.ready) {
		list_next.pop();
		if (entry.type == RoBType::reg) {
			regs.set_val(entry.regId, id, entry.value);
			std::cout << std::format("RoB({}): set reg {} to {}\n", id, entry.regId, entry.value) << std::endl;
		}
		else if (entry.type == RoBType::branch) {
			if (entry.value) {
				newPC_next = entry.jumpAddr;
				clear_signal = true;
				std::cout << std::format("RoB({}): jump to {}\n", id, newPC_next) << std::endl;
			}
			std::cout << std::format("RoB({}): jump hit.\n", id) << std::endl;
		}
		else if (entry.type == RoBType::store) {
			std::cout << std::format("RoB({}): store {:0>8x} done\n", id, entry.instrAddr) << std::endl;
			// do nothing
		}
		else if (entry.type == RoBType::exit) {
			std::cout << std::format("RoB({}): exit\n", id) << std::endl;
			throw regs[10] & 0xff;
		}
	}
}

void ReorderBuffer::add(Instr2RoB const &entry, RegisterUnit &regs) {
	if (list.full()) throw "ReorderBuffer add: list is full";
	regs.set_dependence(entry.regId, list.tail);
	list.push({entry, true, false});
	std::cout << std::format("RoB: add tpye {} instr {:0>8x}", int(entry.type), entry.instrAddr) << std::endl;
}

void ReorderBuffer::on_clear() {
	list_next.clear();
	list_next.head = list_next.tail = 0;
	clear_signal_next = false;
	newPC_next = 0;
}
