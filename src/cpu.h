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
		rs.init(&alu);
		lsb.init();
		alu.init();
		cache.init();
		flush();
	}
	unsigned int work() {
		if (!mem) throw "Memory not linked";
		init();
		unsigned long long tick = 0;
		try {
			for (;; ++tick) {
				execute();
				flush();
			}
		} catch (unsigned int x) {
			std::cerr << "tick: " << tick << std::endl;
			return x;
		} catch (...) {
			throw;
		}
		return 0;
	}
	void execute() {
		// up level
		instruction.execute(*mem, !decoder.ready_next, decoder.set_PC, decoder.newPC, rob.clear_signal, rob.newPC);
		decoder.execute(instruction, rob, rs, lsb, regs);
		rob.execute(regs, rs.export_data(), lsb.export_data());
		lsb.execute(cache, *mem, rs.export_data(), rob.data_to_LSB());
		rs.execute(lsb.export_data(), rob.clear_signal);
		cache.execute(*mem, rob.clear_signal);
		alu.execute();
		regs.execute(rob.clear_signal);
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
