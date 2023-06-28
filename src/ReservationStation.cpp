#include "ReservationStation.h"

void ReservationStation::add(Instr2RS const &entry) {
	int index = -1;
	for (int i = 0; i < list.max_size(); ++i) {
		if (!list[i].busy) {
			index = i;
			break;
		}
	}
	if (index == -1) throw "ReservationStation is full.";
	static_cast<Instr2RS &>(list_next[index]) = entry;
	list_next[index].busy = true;
}

void ReservationStation::execute(ResultType lsb, bool clear_signal) {
	if (clear_signal) {
		on_clear();
		return;
	}

	update_dependencies(lsb);

	int index = next_calc_entry(lsb);
	auto const get_true_i = [alu = *alu, lsb](ReservationStationEntry const &ele) {
		if (!ele.hasDependentI) return ele.Ri;
		if (alu.ready && ele.Qi == alu.RoB_id) return alu.get_result();
		if (lsb.ready && ele.Qi == lsb.RoB_id) return lsb.value;
		throw "Unexpected call to get_true_i";
	};
	auto get_true_j = [alu = *alu, lsb](ReservationStationEntry const &ele) {
		if (!ele.hasDependentJ) return ele.Rj;
		if (alu.ready && ele.Qj == alu.RoB_id) return alu.get_result();
		if (lsb.ready && ele.Qj == lsb.RoB_id) return lsb.value;
		throw "Unexpected call to get_true_j";
	};
	if (index >= 0) {
		alu->set_input(list[index].type, get_true_i(list[index]), get_true_j(list[index]), list[index].RoB_id);
		list_next[index].busy = false;
	}
}

void ReservationStation::update_dependencies(ResultType lsb) {
	for (int i = 0; i < list.max_size(); ++i) {
		if (!list[i].busy) continue;
		if (alu->ready) {
			if (list[i].hasDependentI && list[i].Qi == alu->RoB_id) {
				list_next[i].Qi = 0;
				list_next[i].Ri = alu->get_result();
				list_next[i].hasDependentI = false;
			}
			if (list[i].hasDependentJ && list[i].Qj == alu->RoB_id) {
				list_next[i].Qj = 0;
				list_next[i].Rj = alu->get_result();
				list_next[i].hasDependentJ = false;
			}
		}
		if (lsb.ready) {
			if (list[i].hasDependentI && list[i].Qi == lsb.RoB_id) {
				list_next[i].Qi = 0;
				list_next[i].Ri = lsb.value;
				list_next[i].hasDependentI = false;
			}
			if (list[i].hasDependentJ && list[i].Qj == lsb.RoB_id) {
				list_next[i].Qj = 0;
				list_next[i].Rj = lsb.value;
				list_next[i].hasDependentJ = false;
			}
		}
	}
}

int ReservationStation::next_calc_entry(ResultType lsb) {
	for (int i = 0; i < list.max_size(); ++i)
		if (list[i].busy &&
			(!list[i].hasDependentI || (lsb.ready && list[i].Qi == lsb.RoB_id) || (alu->ready && list[i].Qi == alu->RoB_id)) &&
			(!list[i].hasDependentJ || (lsb.ready && list[i].Qj == lsb.RoB_id) || (alu->ready && list[i].Qj == alu->RoB_id)))
			return i;
	return -1;
}

bool ReservationStation::full() {
	for (auto &ele: list)
		if (!ele.busy) return false;
	return true;
}

void ReservationStation::on_clear() {
	for (auto &x: list_next) x.busy = false;
	alu->set_input(Calc_type::hang, 0, 0, 0);
}

void ReservationStation::init(ALU *_alu) {
	alu = _alu;
	for (auto &x: list_next) x.busy = false;
}
