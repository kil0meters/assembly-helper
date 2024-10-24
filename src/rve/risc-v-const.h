#pragma once
#include <stdint.h>
// #include "inst.h"
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;
typedef u8 b8;

#define B8_TRUE 1
#define B8_FALSE 0

enum rve_instruction_type {
	risc_v_r_type = 0,
	risc_v_i_type,
	risc_v_s_type,
	risc_v_b_type,
	risc_v_u_type,
	risc_v_j_type,
};

#define RVE_NUM_SUPPORTED_OPCODES 10

enum RVE_OPCODES {
	RVE_OPCODE_LUI = 0,
	RVE_OPCODE_AUIPC,
	RVE_OPCODE_JAL,
	RVE_OPCODE_JALR,
	RVE_OPCODE_B,
	RVE_OPCODE_L,
	RVE_OPCODE_S,
	RVE_OPCODE_MATH_IMM,
	RVE_OPCODE_MATH,
};

enum RVE_OPCODE_VALS {
	RVE_OPCODE_VAL_LUI = 0b0110111, /// LUI
	RVE_OPCODE_VAL_AUIPC = 0b0010111, /// AUIPC
	RVE_OPCODE_VAL_JAL = 0b1101111, /// JAL
	RVE_OPCODE_VAL_JALR = 0b1100111, /// JALR
	RVE_OPCODE_VAL_B = 0b1100011, /// B
	RVE_OPCODE_VAL_L = 0b0000011, /// L
	RVE_OPCODE_VAL_S = 0b0100011, /// S
	RVE_OPCODE_VAL_MATH_IMM = 0b0010011, /// MATH_IMM
	RVE_OPCODE_VAL_MATH = 0b0110011, /// MATH
};

static u8 RVE_OPCODE_VALS_ARR[RVE_NUM_SUPPORTED_OPCODES] = {
	RVE_OPCODE_VAL_LUI,
	RVE_OPCODE_VAL_AUIPC,
	RVE_OPCODE_VAL_JAL,
	RVE_OPCODE_VAL_JALR,
	RVE_OPCODE_VAL_B,
	RVE_OPCODE_VAL_L,
	RVE_OPCODE_VAL_S,
	RVE_OPCODE_VAL_MATH_IMM,
	RVE_OPCODE_VAL_MATH,
};

static enum rve_instruction_type RVE_OPCODE_TYPES[RVE_NUM_SUPPORTED_OPCODES] = {
	risc_v_u_type, /// LUI
	risc_v_u_type, /// AUIPC
	risc_v_j_type, /// JAL
	risc_v_i_type, /// JALR
	risc_v_b_type, /// B
	risc_v_i_type, /// L
	risc_v_s_type, /// S
	risc_v_i_type, /// MATH_IMM
	risc_v_r_type, /// MATH
};

enum RVE_B_FUNCT3 {
	RVE_B_FUNCT3_BEQ = 0b000,
	RVE_B_FUNCT3_BNE = 0b001,
	RVE_B_FUNCT3_BLT = 0b100,
	RVE_B_FUNCT3_BGE = 0b101,
	RVE_B_FUNCT3_BLTU = 0b110,
	RVE_B_FUNCT3_BGEU = 0b111
};

enum RVE_L_FUNCT3 {
	RVE_L_FUNCT3_LB = 0b000,
	RVE_L_FUNCT3_LH = 0b001,
	RVE_L_FUNCT3_LW = 0b010,
	RVE_L_FUNCT3_LBU = 0b100,
	RVE_L_FUNCT3_LHU = 0b101,
};

enum RVE_S_FUNCT3 {
	RVE_S_FUNCT3_SB = 0b000,
	RVE_S_FUNCT3_SH = 0b001,
	RVE_S_FUNCT3_SW = 0b010,
};


enum RVE_OP_MATH_FUNCT3 {
	RVE_OP_MATH_FUNCT3_ADD = 0b000,
	RVE_OP_MATH_FUNCT3_SUB = 0b000,
	RVE_OP_MATH_FUNCT3_SLT = 0b010,
	RVE_OP_MATH_FUNCT3_SLTU = 0b011,
	RVE_OP_MATH_FUNCT3_XOR = 0b100,
	RVE_OP_MATH_FUNCT3_SLL = 0b001,
	RVE_OP_MATH_FUNCT3_SRL = 0b101,
	RVE_OP_MATH_FUNCT3_SRA = 0b101,
	RVE_OP_MATH_FUNCT3_OR = 0b110,
	RVE_OP_MATH_FUNCT3_AND = 0b111,
};
