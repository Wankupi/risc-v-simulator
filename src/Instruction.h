#pragma once
#include "Memory.h"

enum class Opera {
	lui,
	auipc,
	jal,
	jalr,
	beq,
	bne,
	blt,
	bge,
	bltu,
	bgeu,
	lb,
	lh,
	lw,
	lbu,
	lhu,
	sb,
	sh,
	sw,
	addi,
	slti,
	sltiu,
	xori,
	ori,
	andi,
	slli,
	srli,
	srai,
	add,
	sub,
	sll,
	slt,
	sltu,
	xor_,
	srl,
	sra,
	or_,
	and_
};

class InstructionUnit {
public:
	void execute(Memory &mem, bool block);

	void flush() {
		PC = PC_next;
		wait1 = wait1_next;
		ready = ready_next;
		op = op_next;
		RS1 = RS1_next;
		RS2 = RS2_next;
		IMM = IMM_next;
		RD = RD_next;
		instrAddr = instrAddr_next;
	}

public:
	unsigned PC;// Program Counter
	bool wait1;
	bool ready;
	Opera op;
	unsigned RS1, RS2, IMM, RD;
	unsigned instrAddr;

private:
	unsigned PC_next;
	bool wait1_next;
	bool ready_next;
	Opera op_next;
	unsigned RS1_next, RS2_next, IMM_next, RD_next;
	unsigned instrAddr_next;
};
