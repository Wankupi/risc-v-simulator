#pragma once
#include "ALU.h"
#include "bus.h"
#include <array>

struct ReservationStationEntry : public Instr2RS {
	bool busy;
};

class ReservationStation {
public:
	void add(Instr2RS const &entry);
	void execute(ALU &alu, ResultType lsb);

	void flush() {
		list = list_next;
		result = result_next;
	}
	bool full();
	ResultType export_data(ALU &alu) const {
		return result;
	}

private:
	void update_dependencies(ALU &alu, ResultType lsb);

	/// @attention In simulator, it return -1 for not_found, but in real hardware, there should be another wire.
	int next_calc_entry(ALU &alu, ResultType lsb);

public:
	std::array<ReservationStationEntry, 16> list;
	ResultType result;

private:
	std::array<ReservationStationEntry, 16> list_next;
	ResultType result_next;
};
