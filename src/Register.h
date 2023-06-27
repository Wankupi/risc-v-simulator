#pragma once
#include <array>

class RegisterUnit {
public:
	void flush() {
		R = R_next;
		R[0] = 0;
		Q = Q_next;
		D = D_next;
		D[0] = false;
	}
	unsigned int operator[](unsigned int x) { return R[x]; }
	unsigned int get_dependence(unsigned int x) { return Q[x]; }
	bool has_dependence(unsigned int x) { return D[x]; }
	void set_dependence(unsigned int x, unsigned int RoB_id) {
		D_next[x] = true;
		Q_next[x] = RoB_id;
	}
	void set_val(unsigned int x, unsigned int RoB_id, unsigned int val) {
		R_next[x] = val;
		D_next[x] = (Q[x] != RoB_id);
		if (Q[x] == RoB_id) Q_next[x] = 0;
	}
	void init() {
		R_next.fill(0);
		Q_next.fill(0);
		D_next.fill(0);
	}

public:
	std::array<unsigned int, 32> R;
	std::array<unsigned int, 32> Q;
	std::array<bool, 32> D;

private:
	std::array<unsigned int, 32> R_next;
	std::array<unsigned int, 32> Q_next;
	std::array<bool, 32> D_next;
};