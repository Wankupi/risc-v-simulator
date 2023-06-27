#pragma once
#include "bus.h"

/*
 * ALU 设计
 * 计算需要一个周期
 * input ready
 * input reg[2,0] type;
 * input reg[3,0] RoB_id;
 * input reg [31,0] in1, in2;
 * output wire ready
 * output wire result
 * output wire RoB_id;
 * 写入的内容在下一个周期进行计算
 */

struct ALU {
	void set_input(Calc_type calc_type, unsigned int A, unsigned int B, unsigned int RoB_id_) {
		type_next = calc_type;
		in1_next = A;
		in2_next = B;
		RoB_id_next = RoB_id_;
	}
	unsigned int get_result() {
		if (!ready)
			return 0;
		if (type == Calc_type::add)
			return in1 + in2;
		else if (type == Calc_type::sub)
			return in1 - in2;
		else if (type == Calc_type::shiftL)
			return in1 << in2;
		else if (type == Calc_type::shiftR)
			return in1 >> in2;
		else if (type == Calc_type::shiftRa)
			return static_cast<signed int>(in1) >> in2;
		else if (type == Calc_type::xor_)
			return in1 ^ in2;
		else if (type == Calc_type::and_)
			return in1 & in2;
		else if (type == Calc_type::or_)
			return in1 | in2;
		else if (type == Calc_type::equal)
			return in1 == in2;
		else if (type == Calc_type::notEqual)
			return in1 != in2;
		else if (type == Calc_type::less)
			return static_cast<signed int>(in1) < static_cast<signed int>(in2);
		else if (type == Calc_type::lessUnsigned)
			return static_cast<unsigned int>(in1) < static_cast<unsigned int>(in2);
		else if (type == Calc_type::greaterEq)
			return static_cast<signed int>(in1) >= static_cast<signed int>(in2);
		else if (type == Calc_type::greaterEqUnsigned)
			return static_cast<unsigned int>(in1) >= static_cast<unsigned int>(in2);
		else
			return 0;
	}
	void flush() {
		ready = ready_next;
		ready_next = false;
		type = type_next;
		in1 = in1_next;
		in2 = in2_next;
		RoB_id = RoB_id_next;
	}

public:
	bool ready;
	Calc_type type;
	unsigned int in1, in2;
	unsigned int RoB_id;

private:
	bool ready_next;
	Calc_type type_next;
	unsigned int in1_next, in2_next;
	unsigned int RoB_id_next;
};
