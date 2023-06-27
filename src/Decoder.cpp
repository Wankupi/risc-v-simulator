#include "Decoder.h"

void DecoderUnit::execute(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {
	if (!iu.ready)
		return;
	ready_next = true;// default
	isAddRob = false;
	isAddLSB = false;
	isAddRS = false;
	switch (iu.op) {
		case Opera::lui: func_lui(iu, rob, rs, lsb); break;
		case Opera::auipc: func_auipc(iu, rob, rs, lsb); break;
		case Opera::jal: func_jal(iu, rob, rs, lsb); break;
		case Opera::jalr: func_jalr(iu, rob, rs, lsb); break;
		case Opera::beq: func_beq(iu, rob, rs, lsb); break;
		case Opera::bne: func_bne(iu, rob, rs, lsb); break;
		case Opera::blt: func_blt(iu, rob, rs, lsb); break;
		case Opera::bge: func_bge(iu, rob, rs, lsb); break;
		case Opera::bltu: func_bltu(iu, rob, rs, lsb); break;
		case Opera::bgeu: func_bgeu(iu, rob, rs, lsb); break;
		case Opera::lb: func_lb(iu, rob, rs, lsb); break;
		case Opera::lh: func_lh(iu, rob, rs, lsb); break;
		case Opera::lw: func_lw(iu, rob, rs, lsb); break;
		case Opera::lbu: func_lbu(iu, rob, rs, lsb); break;
		case Opera::lhu: func_lhu(iu, rob, rs, lsb); break;
		case Opera::sb: func_sb(iu, rob, rs, lsb); break;
		case Opera::sh: func_sh(iu, rob, rs, lsb); break;
		case Opera::sw: func_sw(iu, rob, rs, lsb); break;
		case Opera::addi: func_addi(iu, rob, rs, lsb); break;
		case Opera::slti: func_slti(iu, rob, rs, lsb); break;
		case Opera::sltiu: func_sltiu(iu, rob, rs, lsb); break;
		case Opera::xori: func_xori(iu, rob, rs, lsb); break;
		case Opera::ori: func_ori(iu, rob, rs, lsb); break;
		case Opera::andi: func_andi(iu, rob, rs, lsb); break;
		case Opera::slli: func_slli(iu, rob, rs, lsb); break;
		case Opera::srli: func_srli(iu, rob, rs, lsb); break;
		case Opera::srai: func_srai(iu, rob, rs, lsb); break;
		case Opera::add: func_add(iu, rob, rs, lsb); break;
		case Opera::sub: func_sub(iu, rob, rs, lsb); break;
		case Opera::sll: func_sll(iu, rob, rs, lsb); break;
		case Opera::slt: func_slt(iu, rob, rs, lsb); break;
		case Opera::sltu: func_sltu(iu, rob, rs, lsb); break;
		case Opera::xor_: func_xor_(iu, rob, rs, lsb); break;
		case Opera::srl: func_srl(iu, rob, rs, lsb); break;
		case Opera::sra: func_sra(iu, rob, rs, lsb); break;
		case Opera::or_: func_or(iu, rob, rs, lsb); break;
		case Opera::and_: func_and(iu, rob, rs, lsb); break;
	}
}

void DecoderUnit::func_lui(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {
	if (rob.full()) {
		ready_next = false;
		return;
	}
	toRob = {RoBType::reg, iu.RD, iu.IMM, iu.instrAddr, 0};
	isAddRob = true;
}

void DecoderUnit::func_auipc(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {
	if (rob.full()) {
		ready_next = false;
		return;
	}
	toRob = {RoBType::reg, iu.RD, iu.instrAddr + iu.IMM, iu.instrAddr, 0};
	isAddRob = true;
}

void DecoderUnit::func_jal(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {
	if (rob.full()) {
		ready_next = false;
		return;
	}
	toRob = {RoBType::reg, iu.RD, iu.PC, iu.instrAddr, 0};
	isAddRob = true;
	set_PC_next = true;
	newPC_next = iu.PC + (static_cast<int>(iu.IMM << 11) >> 11);
}

void DecoderUnit::func_jalr(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {
	//TODO:jalr
	if (rob.full()) {
		ready_next = false;
		return;
	}
	toRob = {RoBType::reg, iu.RD, iu.PC, iu.instrAddr, 0};
	isAddRob = true;
	set_PC = true;
	newPC = (iu.RS1 + (static_cast<int>(iu.IMM << 20) >> 20)) & 0xfffffffe;
}

void DecoderUnit::func_beq(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_bne(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_blt(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_bge(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_bltu(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_bgeu(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_lb(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_lh(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_lw(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_lbu(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_lhu(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_sb(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_sh(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_sw(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_addi(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_slti(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_sltiu(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_xori(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_ori(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_andi(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_slli(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_srli(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_srai(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}

void DecoderUnit::func_add(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {
	if (rob.full() || rs.full()) {
		ready_next = false;
		return;
	}
	toRob = {RoBType::reg, iu.RD, 0, iu.instrAddr, 0};
	toRS = {Calc_type::add, };
}

void DecoderUnit::func_sub(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_sll(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_slt(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_sltu(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_xor_(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_srl(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_sra(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_or(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}
void DecoderUnit::func_and(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb) {}

