#pragma once
#include <array>

class Predictor {
public:
	enum PredictorStat { not_strong,
						 not_weak,
						 yes_weak,
						 yes_strong };
	void init() {
		stat.fill(yes_weak);
		total = success = 0;
	}
	bool get_prediction(unsigned pc) {
		return stat[hash(pc)] == PredictorStat::yes_weak || stat[hash(pc)] == PredictorStat::yes_strong;
	}
	void update(unsigned pc, bool ok) {
		++total;
		if (ok) ++success;
		bool jumped = (get_prediction(pc) == ok);
		int ind = hash(pc);
		switch (stat[ind]) {
			case not_strong:
				if (jumped) stat[ind] = not_weak;
				break;
			case not_weak:
				if (jumped) stat[ind] = yes_weak;
				else
					stat[ind] = not_strong;
				break;
			case yes_weak:
				if (jumped) stat[ind] = yes_strong;
				else
					stat[ind] = not_weak;
				break;
			case yes_strong:
				if (!jumped) stat[ind] = yes_weak;
				break;
		}
	}
	void flush() {}

private:
	constexpr static int SIZE = 1 << 10;
	inline int hash(unsigned pc) {
		return (pc >> 2) & (SIZE - 1);
	}

public:
	long long total;
	long long success;

private:
	std::array<PredictorStat, SIZE> stat;
};