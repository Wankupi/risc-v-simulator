#include "Decoder.h"
#include "Instruction.h"
#include "LoadStoreBuffer.h"
#include "ReorderBuffer.h"
#include "ReservationStation.h"
#include <chrono>
#include <functional>
#include <random>

class CPU {
public:
	void link_memory(Memory *pMemory) { this->mem = pMemory; }
	void init() {
		regs.init();
		instruction.init();
		decoder.init(&predictor);
		rob.init();
		rs.init(&alu);
		lsb.init(&cache);
		alu.init();
		cache.init(mem);
		predictor.init();
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
			std::cerr << "predictor: " << predictor.success << " / " << predictor.total << " = " << static_cast<long double>(predictor.success) / predictor.total << std::endl;
			return x;
		} catch (...) {
			throw;
		}
		throw "cpu loop is terminated.";
	}
	void execute() {
		// up level
		static int a[8] = {0, 1, 2, 3, 4, 5, 6, 7};
		static std::function<void()> func[] = {
				[this]() { instruction.execute(*mem, !decoder.ready_next, decoder.set_PC, decoder.newPC, rob.clear_signal, rob.newPC); },
				[this]() { decoder.execute(instruction, rob, rs, lsb, regs); },
				[this]() { rob.execute(regs, rs.export_data(), lsb.export_data(), predictor); },
				[this]() { lsb.execute(rs.export_data(), rob.data_to_LSB()); },
				[this]() { rs.execute(lsb.export_data(), rob.clear_signal); },
				[this]() { cache.execute(rob.clear_signal); },
				[this]() { alu.execute(); },
				[this]() { regs.execute(rob.clear_signal); }};
		static std::default_random_engine e(std::chrono::system_clock::now().time_since_epoch().count());
		std::shuffle(a, a + 8, e);
		for (auto i: a) func[i]();
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
		predictor.flush();
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
	Predictor predictor;
};
