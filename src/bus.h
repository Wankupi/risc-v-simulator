#pragma once

enum class Calc_type {
	add,
	sub,
	shiftL,
	shiftR,
	shiftRa,
	xor_,
	and_,
	or_,
	equal,
	notEqual,
	less,
	lessUnsigned,
	greaterEq,
	greaterEqUnsigned,
	hang
};

enum class LS_type {
	load_word,
	load_half,
	load_byte,
	load_half_unsigned,
	load_byte_unsigned,
	store_word,
	store_half,
	store_byte,
	hang
};


struct ResultType {
	bool ready;
	unsigned int RoB_id;
	unsigned int value;
};


struct Instr2RS {
	Calc_type type;
	unsigned int Ri, Rj, Qi, Qj;
	unsigned int RoB_id;// RoB index
	bool hasDependentI, hasDependentJ;
};

struct Instr2LSB {
	LS_type type;
	unsigned int Ri, Rj;
	unsigned int Qi, Qj;
	unsigned int offset;
	unsigned int RoB_id;
	bool hasDependentI, hasDependentJ;
	bool wait;
};

struct RoB2LSB {
	bool clear_signal;
	bool ready;
	unsigned firstID;
};

enum class RoBType { reg,
					 store,
					 branch,
					 exit };
struct Instr2RoB {
	RoBType type;
	unsigned int regId;
	unsigned int value;
	unsigned int instrAddr; // used for predictor
	unsigned int jumpAddr;
	bool ready;
};
