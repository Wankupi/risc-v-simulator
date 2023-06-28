#pragma once
#include "Instruction.h"
#include "LoadStoreBuffer.h"
#include "Register.h"
#include "ReorderBuffer.h"
#include "ReservationStation.h"
#include "bus.h"

class DecoderUnit {
public:
	void execute(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb, RegisterUnit &regs);
	void flush() {
		set_PC = set_PC_next;
		newPC = newPC_next;
		ready_next = true;
	}
	void init() {
		set_PC_next = false;
		newPC_next = 0;
		ready_next = true;
	}
private:
	void func_lui(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb, RegisterUnit &regs);
	void func_auipc(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb, RegisterUnit &regs);
	void func_jal(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb, RegisterUnit &regs);
	void func_jalr(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb, RegisterUnit &regs);
	void func_branch(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb, RegisterUnit &regs);
	void func_load(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb, RegisterUnit &regs);
	void func_store(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb, RegisterUnit &regs);
	void func_calc(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb, RegisterUnit &regs);
	void func_calc_imm(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb, RegisterUnit &regs);
	void func_exit(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb, RegisterUnit &regs);
	Calc_type to_calc_type(Opera op);
	LS_type to_ls_type(Opera op);
	Calc_type to_calc_type_from_branch(Opera op);
	void deal_dependent(ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb, RegisterUnit &regs);

public:
	bool set_PC;
	unsigned newPC;

private:
	bool set_PC_next;
	unsigned newPC_next;

private:// shared variables in functions, do no realy exists in hardware
	Instr2RoB toRob;
	Instr2LSB toLSB;
	Instr2RS toRS;
	bool isAddRob = false, isAddLSB = false, isAddRS = false;

public:
	bool ready_next;
};
