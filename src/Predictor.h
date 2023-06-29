#pragma once
#include <array>

class Predictor {
public:
	enum PredictorStat { not_strong,
						 not_weak,
						 yes_weak,
						 yes_strong };
	void init() {
		stat.fill({});
		his.fill(0);
		total = success = 0;
	}
	bool get_prediction(unsigned pc) {
		auto ind = hash(pc);
		return stat[ind][his[ind]] == PredictorStat::yes_weak || stat[ind][his[ind]] == PredictorStat::yes_strong;
	}
	void update(unsigned pc, bool ok) {
		++total;
		if (ok) ++success;
		bool jumped = (get_prediction(pc) == ok);
		int ind = hash(pc);
		auto &st = stat[ind][his[ind]];
		switch (st) {
			case not_strong:
				if (jumped) st = not_weak;
				break;
			case not_weak:
				if (jumped) st = yes_weak;
				else
					st = not_strong;
				break;
			case yes_weak:
				if (jumped) st = yes_strong;
				else
					st = not_weak;
				break;
			case yes_strong:
				if (!jumped) st = yes_weak;
				break;
		}
		his[ind] = ((his[ind] << 1) | jumped) & (HIS_SIZE - 1);
	}
	void flush() {}

private:
	constexpr static int SIZE = 1 << 6, HIS_SIZE = 1 << 4;
	inline int hash(unsigned pc) {
		return (pc >> 2) & (SIZE - 1);
	}

public:
	long long total;
	long long success;

private:
	std::array<int, SIZE> his;
	std::array<std::array<PredictorStat, HIS_SIZE>, SIZE> stat;
};
