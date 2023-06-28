#include "LoadStoreBuffer.h"

void LoadStoreBuffer::execute(Cache &cache, Memory &mem, ResultType rs, RoB2LSB rob) {
	if (rob.clear_signal) {
		on_clear();
		return;
	}
	auto cr = cache.get_result(mem);
	result_next = cr;
	update_dependencies(cr, rs, rob);

	auto const get_true_i = [cr, rs](LSB_entry const &ele) {
		if (!ele.hasDependentI) return ele.Ri;
		if (cr.ready && ele.Qi == cr.RoB_id) return cr.value;
		if (rs.ready && ele.Qi == rs.RoB_id) return rs.value;
		throw "Unexpected call to get_true_i";
	};
	auto const get_true_j = [cr, rs](LSB_entry const &ele) {
		if (!ele.hasDependentJ) return ele.Rj;
		if (cr.ready && ele.Qj == cr.RoB_id) return cr.value;
		if (rs.ready && ele.Qj == rs.RoB_id) return rs.value;
		throw "Unexpected call to get_true_j";
	};
	if (cache.free()) {
		int index = next_issue_entry(cr, rs, rob);
		if (index != -1) {
			list_next[index].busy = false;
			auto const &ele = list[index];
			unsigned int RItrue = get_true_i(ele), RJtrue = get_true_j(ele);
			cache.set(RItrue + ele.offset, RJtrue, ele.RoB_id, ele.type);
			// both ok for load and store. RJ is not used when loading.
		}
	}
}

void LoadStoreBuffer::add(const Instr2LSB &entry) {
	int index = -1;
	for (int i = 0; i < list.max_size(); ++i) {
		if (!list[i].busy) {
			index = i;
			break;
		}
	}
	if (index == -1) throw "LoadStoreBuffer is full.";
	static_cast<Instr2LSB &>(list_next[index]) = entry;
	list_next[index].busy = true;
}

bool LoadStoreBuffer::full() {
	for (auto &ele: list)
		if (ele.busy && (ele.type == LS_type::store_word || ele.type == LS_type::store_half || ele.type == LS_type::store_byte))
			return true;
	for (auto &ele: list) {
		if (!ele.busy) return false;
	}
	return true;
}

int LoadStoreBuffer::next_issue_entry(ResultType lsb, ResultType rs, RoB2LSB rob) {
	for (int i = 0; i < list.max_size(); ++i)
		if (list[i].busy &&
			(!list[i].hasDependentI || (lsb.ready && list[i].Qi == lsb.RoB_id) || (rs.ready && list[i].Qi == rs.RoB_id)) &&
			(!list[i].hasDependentJ || (lsb.ready && list[i].Qj == lsb.RoB_id) || (rs.ready && list[i].Qj == rs.RoB_id)) &&
			(!list[i].wait || list[i].RoB_id == rob.firstID))
			return i;
	return -1;
}

void LoadStoreBuffer::update_dependencies(ResultType cr, ResultType rs, RoB2LSB rob) {
	for (int i = 0; i < list.max_size(); ++i) {
		if (!list[i].busy) continue;
		if (cr.ready && list[i].hasDependentI && list[i].Qi == cr.RoB_id) {
			list_next[i].hasDependentI = false;
			list_next[i].Qi = 0;
			list_next[i].Ri = cr.value;
		}
		if (cr.ready && list[i].hasDependentJ && list[i].Qj == cr.RoB_id) {
			list_next[i].hasDependentJ = false;
			list_next[i].Qj = 0;
			list_next[i].Rj = cr.value;
		}
		if (rs.ready && list[i].hasDependentI && list[i].Qi == rs.RoB_id) {
			list_next[i].hasDependentI = false;
			list_next[i].Qi = 0;
			list_next[i].Ri = rs.value;
		}
		if (rs.ready && list[i].hasDependentJ && list[i].Qj == rs.RoB_id) {
			list_next[i].hasDependentJ = false;
			list_next[i].Qj = 0;
			list_next[i].Rj = rs.value;
		}
		if (rob.ready && list[i].wait && list[i].RoB_id == rob.firstID)
			list_next[i].wait = false;
	}
}

void LoadStoreBuffer::on_clear() {
	for (auto &x: list_next) x.busy = false;
	result_next.ready = false;
}
