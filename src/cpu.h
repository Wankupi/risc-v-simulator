#include "Decoder.h"
#include "Instruction.h"
#include "LoadStoreBuffer.h"
#include "ReorderBuffer.h"
#include "ReservationStation.h"

class CPU {
public:
	void link_memory(Memory *pMemory) { this->mem = pMemory; }
	void init() {
		regs.init();
		instruction.init();
		decoder.init();
		rob.init();
		rs.init();
		lsb.init();
		alu.init();
		cache.init();
		flush();
	}
	int work() {
		if (!mem) throw "Memory not linked";
		init();
		try {
			for (unsigned int tick = 1;; ++tick) {
				std::cout << "time clock : " << tick << " - - - - - - - - -" << std::endl;
				execute();
				flush();
				std::cout << std::endl;
			}
		} catch (int x) {
			return x;
		} catch (...) {
			throw;
		}
	}
	void execute() {
		// up level
		instruction.execute(*mem, !decoder.ready_next, decoder.set_PC, decoder.newPC, rob.clear_signal, rob.newPC);
		decoder.execute(instruction, rob, rs, lsb, regs);
		rob.execute(regs, rs.export_data(alu), lsb.export_data());
		lsb.execute(cache, *mem, rs.export_data(alu), rob.data_to_LSB());
		rs.execute(alu, lsb.export_data(), rob.clear_signal);
		cache.execute(*mem, rob.clear_signal);
		alu.execute();
	}
	void flush() {
		// down level
		instruction.flush();
		decoder.flush();
		rob.flush();
		alu.flush();
		lsb.flush();
		cache.flush();
		rs.flush();
		regs.flush();
	}

private:
	RegisterUnit regs;
	Memory *mem;// memory does not belong to CPU
	InstructionUnit instruction;
	DecoderUnit decoder;
	ReorderBuffer rob;
	ReservationStation rs;
	LoadStoreBuffer lsb;
	ALU alu;
	Cache cache;
};
