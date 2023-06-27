#include "Memory.h"
#include "Control.h"
#include <fstream>


int main() {
	std::ifstream file("t.in", std::ios::in);

	Memory mem;
	RegisterUnit regs;
	Cache cache;
	InstructionUnit instruction;
	DecoderUnit decoder;
	ReorderBuffer rob;
	ReservationStation rs;
	LoadStoreBuffer lsb;
	ALU alu;

	mem.load_file(file);
	unsigned long clock = 0;
	while (true) {
		++clock;
		// up level
		instruction.execute(mem, false);
//		decoder.execute(instruction.IR, );

		rs.execute(alu,  lsb.export_data());


		// down level
		instruction.flush();
		decoder.flush();
		rob.flush();
		alu.flush();
		lsb.flush();
		cache.flush();
		rs.flush();
	}
	return 0;
}
