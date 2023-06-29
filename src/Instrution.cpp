#include "Instruction.h"

void InstructionUnit::execute(Memory &mem, bool decoder_need_stall,
							  bool jump_by_decoder, unsigned int newPC_from_decoder, bool jump_by_RoB, unsigned int newPC_from_RoB) {
	if (!jump_by_RoB) {
		if (decoder_need_stall) {
			ready_next = true;
			return;
		}
		if (wait1) {
			ready_next = false;
			wait1_next = false;
			return;
		}
	}
	unsigned int usedPC = jump_by_RoB ? newPC_from_RoB : (jump_by_decoder ? newPC_from_decoder : PC);
	unsigned IR = mem.load_word(usedPC);

	PC_next = usedPC + 4;
	instrAddr_next = usedPC;
	ready_next = true;
	RD_next = (IR >> 7) & 0b11111;
	RS1_next = (IR >> 15) & 0b11111;
	RS2_next = (IR >> 20) & 0b11111;

	// set IMM
	switch (IR & 0b1111111) {
			// type R
		case 0b0110011:
			IMM_next = 0;
			break;
			// type I
		case 0b1100111:
		case 0b0000011:
		case 0b0010011:
		case 0b0001111:
			IMM_next = (static_cast<int>(IR) >> 20);
			break;
			// type S
		case 0b0100011:
			IMM_next = ((IR >> 7) & 0b11111) | ((IR >> 20) & 0b111111100000);
			IMM_next = (static_cast<int>(IMM_next << 20) >> 20);
			break;
			// type B
		case 0b1100011:
			IMM_next = ((IR >> 7) & 0b11110) | (((IR >> 7) & 1) << 11) | (((IR >> 25) & 0b111111) << 5) | (((IR >> 31) & 1) << 12);
			IMM_next = static_cast<int>(IMM_next << 19) >> 19;
			break;
			// type U
		case 0b0110111:
			IMM_next = IR & (~0b111111111111u);
			// already occupy the 31st bit
			break;
			// type J
		case 0b1101111:
			IMM_next = (((IR >> 21) & 0x3ff) << 1) | (((IR >> 20) & 1) << 11) | (((IR >> 12) & 0xff) << 12) | (((IR >> 31) & 1) << 20);
			IMM_next = static_cast<int>(IMM_next << 11) >> 11;
			break;
		default:
			throw "Unknown instruction.";
	}
	if (IR == 0x0ff00513) {
		op_next = Opera::exit;
		return;
	}
	// set op
	switch (IR & 0x7f) {
		case 0b0110111: op_next = Opera::lui; break;
		case 0b0010111: op_next = Opera::auipc; break;
		case 0b1101111:
			op_next = Opera::jal;
			wait1_next = true;
			break;
		case 0b1100111:
			op_next = Opera::jalr;
			wait1_next = true;
			break;
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

void InstructionUnit::stall_by_decoder() {
	// cancel all the next stage
	PC_next = PC;
	wait1_next = wait1;
	ready_next = ready;
	op_next = op;
	RS1_next = RS1;
	RS2_next = RS2;
	IMM_next = IMM;
	RD_next = RD;
	instrAddr_next = instrAddr;
}
