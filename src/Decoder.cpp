#include "Decoder.h"

void DecoderUnit::execute(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb, RegisterUnit &regs) {
	set_PC_next = false;
	newPC_next = 0;
	if (rob.clear_signal) {
		ready_next = true;
		return;
	}
	if (!iu.ready)
		return;
	ready_next = true;// default
	set_PC_next = false;
	isAddRob = false;
	isAddLSB = false;
	isAddRS = false;
	switch (iu.op) {
		case Opera::lui: func_lui(iu, rob, rs, lsb, regs); break;
		case Opera::auipc: func_auipc(iu, rob, rs, lsb, regs); break;
		case Opera::jal: func_jal(iu, rob, rs, lsb, regs); break;
		case Opera::jalr: func_jalr(iu, rob, rs, lsb, regs); break;
		case Opera::beq:
		case Opera::bne:
		case Opera::blt:
		case Opera::bge:
		case Opera::bltu:
		case Opera::bgeu: func_branch(iu, rob, rs, lsb, regs); break;
		case Opera::lb:
		case Opera::lh:
		case Opera::lw:
		case Opera::lbu:
		case Opera::lhu: func_load(iu, rob, rs, lsb, regs); break;
		case Opera::sb:
		case Opera::sh:
		case Opera::sw: func_store(iu, rob, rs, lsb, regs); break;
		case Opera::addi:
		case Opera::slti:
		case Opera::sltiu:
		case Opera::xori:
		case Opera::ori:
		case Opera::andi:
		case Opera::slli:
		case Opera::srli:
		case Opera::srai: func_calc_imm(iu, rob, rs, lsb, regs); break;
		case Opera::add:
		case Opera::sub:
		case Opera::sll:
		case Opera::slt:
		case Opera::sltu:
		case Opera::xor_:
		case Opera::srl:
		case Opera::sra:
		case Opera::or_:
		case Opera::and_: func_calc(iu, rob, rs, lsb, regs); break;
		case Opera::exit: func_exit(iu, rob, rs, lsb, regs); break;
		default:
			throw "Decoder: Unknown instruction!";
	}
	deal_dependent(rob, rs, lsb, regs);
	if (isAddRob)
		rob.add(toRob, regs);
	if (isAddRS)
		rs.add(toRS);
	if (isAddLSB)
		lsb.add(toLSB);
	if (!ready_next) {
		iu.stall_by_decoder();
	}
}

void DecoderUnit::func_lui(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb, RegisterUnit &regs) {
	if (rob.full()) {
		ready_next = false;
		return;
	}
	toRob = {RoBType::reg, iu.RD, iu.IMM, iu.instrAddr, 0, true};
	isAddRob = true;
}

void DecoderUnit::func_auipc(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb, RegisterUnit &regs) {
	if (rob.full()) {
		ready_next = false;
		return;
	}
	toRob = {RoBType::reg, iu.RD, iu.instrAddr + iu.IMM, iu.instrAddr, 0, true};
	isAddRob = true;
}

void DecoderUnit::func_jal(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb, RegisterUnit &regs) {
	if (rob.full()) {
		ready_next = false;
		return;
	}
	toRob = {RoBType::reg, iu.RD, iu.PC, iu.instrAddr, 0, true};
	isAddRob = true;
	set_PC_next = true;
	newPC_next = iu.instrAddr + iu.IMM;
}

void DecoderUnit::func_jalr(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb, RegisterUnit &regs) {
	// special: when RS1 is not ready, stall.
	if (rob.full() || regs.has_dependence(iu.RS1)) {
		ready_next = false;
		return;
	}
	toRob = {RoBType::reg, iu.RD, iu.PC, iu.instrAddr, 0, true};
	isAddRob = true;
	set_PC_next = true;
	newPC_next = (regs[iu.RS1] + iu.IMM) & 0xfffffffe;
}

void DecoderUnit::func_branch(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb, RegisterUnit &regs) {
	if (rob.full() || rs.full()) {
		ready_next = false;
		return;
	}
	unsigned int predict = iu.instrAddr + 4;
	unsigned int other = iu.instrAddr + iu.IMM;
	toRob = {RoBType::branch, 0, 0, iu.instrAddr, other, false};
	toRS = {to_calc_type_from_branch(iu.op), regs[iu.RS1], regs[iu.RS2], regs.get_dependence(iu.RS1), regs.get_dependence(iu.RS2), rob.add_index(), regs.has_dependence(iu.RS1), regs.has_dependence(iu.RS2)};
	isAddRob = true;
	isAddRS = true;
	set_PC_next = true;
	newPC_next = predict;
}

void DecoderUnit::func_load(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb, RegisterUnit &regs) {
	if (rob.full() || lsb.full()) {
		ready_next = false;
		return;
	}
	toRob = {RoBType::reg, iu.RD, 0, iu.instrAddr, 0, false};
	toLSB = {to_ls_type(iu.op), regs[iu.RS1], 0, regs.get_dependence(iu.RS1), 0,
			 iu.IMM,
			 rob.add_index(), regs.has_dependence(iu.RS1), false, false};
	isAddRob = true;
	isAddLSB = true;
}

void DecoderUnit::func_store(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb, RegisterUnit &regs) {
	if (rob.full() || lsb.full()) {
		ready_next = false;
		return;
	}
	toRob = {RoBType::store, iu.RD, 0, iu.instrAddr, 0, false};
	toLSB = {to_ls_type(iu.op), regs[iu.RS1], regs[iu.RS2], regs.get_dependence(iu.RS1), regs.get_dependence(iu.RS2),
			 iu.IMM,
			 rob.add_index(), regs.has_dependence(iu.RS1), regs.has_dependence(iu.RS2), true};
	isAddRob = true;
	isAddLSB = true;
}

void DecoderUnit::func_calc(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb, RegisterUnit &regs) {
	if (rob.full() || rs.full()) {
		ready_next = false;
		return;
	}
	toRob = {RoBType::reg, iu.RD, 0, iu.instrAddr, 0, false};
	toRS = {to_calc_type(iu.op), regs[iu.RS1], regs[iu.RS2], regs.get_dependence(iu.RS1), regs.get_dependence(iu.RS2),
			rob.add_index(), regs.has_dependence(iu.RS1), regs.has_dependence(iu.RS2)};
	isAddRob = true;
	isAddRS = true;
}

void DecoderUnit::func_calc_imm(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb, RegisterUnit &regs) {
	if (rob.full() || rs.full()) {
		ready_next = false;
		return;
	}
	toRob = {RoBType::reg, iu.RD, 0, iu.instrAddr, 0, false};
	toRS = {to_calc_type(iu.op), regs[iu.RS1], iu.IMM, regs.get_dependence(iu.RS1), 0,
			rob.add_index(), regs.has_dependence(iu.RS1), false};
	isAddRob = true;
	isAddRS = true;
}

void DecoderUnit::func_exit(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb, RegisterUnit &regs) {
	if (rob.full()) {
		ready_next = false;
		return;
	}
	toRob = {RoBType::exit, 0, 0, iu.instrAddr, 0, true};
	isAddRob = true;
	ready_next = false;
}

Calc_type DecoderUnit::to_calc_type(Opera op) {
	switch (op) {
		case Opera::add:
		case Opera::addi: return Calc_type::add;
		case Opera::sub: return Calc_type::sub;
		case Opera::sll:
		case Opera::slli: return Calc_type::shiftL;
		case Opera::slt:
		case Opera::slti: return Calc_type::less;
		case Opera::sltu:
		case Opera::sltiu: return Calc_type::lessUnsigned;
		case Opera::xor_:
		case Opera::xori: return Calc_type::xor_;
		case Opera::srl:
		case Opera::srli: return Calc_type::shiftR;
		case Opera::sra:
		case Opera::srai: return Calc_type::shiftRa;
		case Opera::or_:
		case Opera::ori: return Calc_type::or_;
		case Opera::and_:
		case Opera::andi: return Calc_type::and_;
	}
	throw "to_calc_type: unexpected op";
}

LS_type DecoderUnit::to_ls_type(Opera op) {
	switch (op) {
		case Opera::lb: return LS_type::load_byte;
		case Opera::lh: return LS_type::load_half;
		case Opera::lw: return LS_type::load_word;
		case Opera::lbu: return LS_type::load_byte_unsigned;
		case Opera::lhu: return LS_type::load_half_unsigned;
		case Opera::sb: return LS_type::store_byte;
		case Opera::sh: return LS_type::store_half;
		case Opera::sw: return LS_type::store_word;
	}
	throw "to_ls_type: unexpected op";
}

Calc_type DecoderUnit::to_calc_type_from_branch(Opera op) {
	switch (op) {
		case Opera::beq: return Calc_type::equal;
		case Opera::bne: return Calc_type::notEqual;
		case Opera::blt: return Calc_type::less;
		case Opera::bge: return Calc_type::greaterEq;
		case Opera::bltu: return Calc_type::lessUnsigned;
		case Opera::bgeu: return Calc_type::greaterEqUnsigned;
	}
	throw "to_calc_type_from_branch: unexpected op";
}

void DecoderUnit::deal_dependent(ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb, RegisterUnit &regs) {
	auto a = rs.export_data(), b = lsb.export_data();
	auto deal_one = [&a, &b, &rob](unsigned int &R, unsigned int &Q, bool &has) {
		if (!has) return;
		if (rob.list[Q].ready) {
			R = rob.list[Q].value;
			has = false;
		}
		else if (a.ready && a.RoB_id == Q) {
			R = a.value;
			has = false;
		}
		else if (b.ready && b.RoB_id == Q) {
			R = b.value;
			has = false;
		}
	};
	if (isAddRS) {
		deal_one(toRS.Ri, toRS.Qi, toRS.hasDependentI);
		deal_one(toRS.Rj, toRS.Qj, toRS.hasDependentJ);
	}
	if (isAddLSB) {
		deal_one(toLSB.Ri, toLSB.Qi, toLSB.hasDependentI);
		deal_one(toLSB.Rj, toLSB.Qj, toLSB.hasDependentJ);
	}
}