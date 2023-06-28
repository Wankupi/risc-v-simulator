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
	void execute(ResultType lsb, bool clear_signal);

	void flush() {
		list = list_next;
	}
	bool full();
	ResultType export_data() const {
		return {alu->ready, alu->RoB_id, alu->get_result()};
	}
	void on_clear();
	void init(ALU *_alu);

private:
	void update_dependencies(ResultType lsb);

	/// @attention In simulator, it return -1 for not_found, but in real hardware, there should be another wire.
	int next_calc_entry(ResultType lsb);

public:
	ALU *alu;
	std::array<ReservationStationEntry, 16> list;

private:
	std::array<ReservationStationEntry, 16> list_next;
};
