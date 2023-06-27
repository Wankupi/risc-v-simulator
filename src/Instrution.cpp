#include "Instruction.h"


void InstructionUnit::execute(Memory &mem, bool block) {
	if (wait1) {
		wait1_next = false;
		return;
	}
	if (block)
		return;

	unsigned IR = mem.load_word(PC);
	PC_next = PC + 4;
	instrAddr_next = PC;
	ready_next = true;
	RD_next = (IR >> 7) & 0b11111;
	RS1_next = (IR >> 15) & 0b11111;
	RS2_next = (IR >> 20) & 0b11111;

	// set IMM
	switch (IR & 0b111111) {
			// type R
		case 0b0110011:
			IMM_next = 0;
			break;
			// type I
		case 0b1100111:
		case 0b0000011:
		case 0b0010011:
		case 0b0001111:
			IMM_next = (IR >> 20);
			break;
			// type S
		case 0b0100011:
			IMM_next = ((IR >> 7) & 0b11111) | ((IR >> 20) & 0b111111100000);
			break;
			// type B
		case 0b1100011:
			IMM_next = ((IR >> 8) & 0b1111) | ((IR >> 7) & 1);
			break;
			// type U
		case 0b0110111:
			IMM_next = IR & (~0b111111111111u);
			break;
			// type J
		case 0b1101111:
			IMM_next = (((IR >> 21) & 0x3ff) << 1) | (((IR >> 20) & 1) << 11) | (((IR >> 12) & 0xff) << 12) | (((IR >> 31) & 1) << 20);
			break;
		default:
			throw "Unknown instruction.";
	}

	// set op
	switch (IR & 0x3f) {
		case 0b0110111: op_next = Opera::lui; break;
		case 0b0010111: op_next = Opera::auipc; break;
		case 0b1101111: op_next = Opera::jal; break;
		case 0b1100111: op_next = Opera::jalr; break;
		case 0b1100011:// branch
			switch ((IR >> 12) & 0b111) {
				case 0b000: op_next = Opera::beq; break;
				case 0b001: op_next = Opera::bne; break;
				case 0b100: op_next = Opera::blt; break;
				case 0b101: op_next = Opera::bge; break;
				case 0b110: op_next = Opera::bltu; break;
				case 0b111: op_next = Opera::bgeu; break;
				default: throw "Unknown branch instruction.";
			}
			wait1_next = true;
			break;
		case 0b000011:// load
			switch ((IR >> 12) & 0b111) {
				case 0b000: op_next = Opera::lb; break;
				case 0b001: op_next = Opera::lh; break;
				case 0b010: op_next = Opera::lw; break;
				case 0b100: op_next = Opera::lbu; break;
				case 0b101: op_next = Opera::lhu; break;
				default: throw "Unknown load instruction.";
			}
			break;
		case 0b0100011:// store
			switch ((IR >> 12) & 0b111) {
				case 0b000: op_next = Opera::sb; break;
				case 0b001: op_next = Opera::sh; break;
				case 0b010: op_next = Opera::sw; break;
				default: throw "Unknown store instruction.";
			}
			break;
		case 0b0010011:// calc imm
			switch ((IR >> 12) & 0b111) {
				case 0b000: op_next = Opera::addi; break;
				case 0b010: op_next = Opera::slti; break;
				case 0b011: op_next = Opera::sltiu; break;
				case 0b100: op_next = Opera::xori; break;
				case 0b110: op_next = Opera::ori; break;
				case 0b111: op_next = Opera::andi; break;
				case 0b001: op_next = Opera::slli; break;
				case 0b101:
					if ((IR >> 30) & 1)
						op_next = Opera::srai;
					else
						op_next = Opera::srli;
					break;
				default: throw "Unknown calc imm instruction.";
			}
			break;
		case 0b0110011:// calc reg
			switch ((IR >> 12) & 0b111) {
				case 0b000:
					if ((IR >> 30) & 1)
						op_next = Opera::sub;
					else
						op_next = Opera::add;
					break;
				case 0b001: op_next = Opera::sll; break;
				case 0b010: op_next = Opera::slt; break;
				case 0b011: op_next = Opera::sltu; break;
				case 0b100: op_next = Opera::xor_; break;
				case 0b101:
					if ((IR >> 30) & 1)
						op_next = Opera::sra;
					else
						op_next = Opera::srl;
					break;
				case 0b110: op_next = Opera::or_; break;
				case 0b111: op_next = Opera::and_; break;
				default: throw "Unknown calc reg instruction.";
			}
			break;
		default:
			throw "Unknown instruction.";
	}
}