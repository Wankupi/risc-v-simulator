#pragma once
#include "Instruction.h"
#include "LoadStoreBuffer.h"
#include "ReorderBuffer.h"
#include "ReservationStation.h"
#include "bus.h"

class DecoderUnit {
public:
	void execute(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void flush();

private:
	void func_lui(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_auipc(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_jal(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_jalr(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_beq(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_bne(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_blt(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_bge(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_bltu(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_bgeu(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_lb(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_lh(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_lw(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_lbu(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_lhu(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_sb(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_sh(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_sw(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_addi(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_slti(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_sltiu(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_xori(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_ori(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_andi(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_slli(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_srli(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_srai(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_add(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_sub(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_sll(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_slt(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_sltu(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_xor_(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_srl(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_sra(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_or(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);
	void func_and(InstructionUnit &iu, ReorderBuffer &rob, ReservationStation &rs, LoadStoreBuffer &lsb);

public:
	bool ready;
	bool set_PC;
	unsigned newPC;

private:
	bool ready_next;
	bool set_PC_next;
	unsigned newPC_next;

private:// shared variables in functions, do no realy exists in hardware
	Instr2RoB toRob;
	Instr2LSB toLSB;
	Instr2RS toRS;
	bool isAddRob = false, isAddLSB = false, isAddRS = false;
};
