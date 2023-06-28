#pragma once
#include "bus.h"
#include <array>

class RegisterUnit {
public:
	void flush() {
		R = R_next;
		R[0] = 0;
		Q = Q_next;
		D = D_next;
		D[0] = false;
		tem_is_set_dep.fill(false);
	}
	unsigned int operator[](unsigned int x) { return R[x]; }
	unsigned int get_dependence(unsigned int x) { return Q[x]; }
	bool has_dependence(unsigned int x) { return D[x]; }
	void set_dependence(unsigned int x, unsigned int RoB_id) {
		if (x == 0) return;
		D_next[x] = true;
		Q_next[x] = RoB_id;
		tem_is_set_dep[x] = true;
	}
	void set_val(unsigned int x, unsigned int RoB_id, unsigned int val) {
		if (x > 31) throw "Register index out of range!";
		if (x == 0) return;
		R_next[x] = val;
		if (!tem_is_set_dep[x]) {
			D_next[x] = (Q[x] != RoB_id);
			if (Q[x] == RoB_id) Q_next[x] = 0;
		}
	}
	void init() {
		R_next.fill(0);
		Q_next.fill(0);
		D_next.fill(0);
	}
	void execute(bool clear_signal) {
		if (clear_signal) {
			Q_next.fill(0);
			D_next.fill(false);
			return;
		}
	}

public:
	std::array<unsigned int, 32> R;
	std::array<unsigned int, 32> Q;
	std::array<bool, 32> D;
private:
	std::array<unsigned int, 32> R_next;
	std::array<unsigned int, 32> Q_next;
	std::array<bool, 32> D_next;

private:
	// this variable is used to solve the problem that a register is written multiple times in one cycle
	std::array<bool, 32> tem_is_set_dep;
};