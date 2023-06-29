#include "ReorderBuffer.h"

void ReorderBuffer::execute(RegisterUnit &regs, ResultType rs, ResultType lsb, Predictor &predictor) {
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
		}
		else if (entry.type == RoBType::branch) {
			if ((entry.value & 1) ^ (entry.jumpAddr & 1)) {
				newPC_next = entry.jumpAddr & (~1);
				clear_signal_next = true;
				predictor.update(entry.instrAddr, false);
			}
			else
				predictor.update(entry.instrAddr, true);
		}
		else if (entry.type == RoBType::store) {
			// do nothing
		}
		else if (entry.type == RoBType::exit) {
			throw regs[10] & 0xff;
		}
	}
}

void ReorderBuffer::add(Instr2RoB entry, RegisterUnit &regs) {
	if (list.full()) throw "ReorderBuffer add: list is full";
	if (entry.type == RoBType::reg)
		regs.set_dependence(entry.regId, list.tail);
	list_next.push({entry, true});
}

void ReorderBuffer::on_clear() {
	list_next.clear();
	clear_signal_next = false;
	newPC_next = 0;
}
