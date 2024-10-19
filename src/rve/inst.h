#pragma once
#include "risc-v-const.h"
#include "exec.h"


#define RVE_SIGN_EXTEND(v, sb) ((v) | (((v) & (1 << (sb))) ? ~((1 << (sb))-1) : 0))


typedef struct {
	u8 funct_7;
	u8 rs2;
	u8 rs1;
	u8 funct_3;
	u8 rd;
	u8 opcode;
	u8 padding[2];
} rve_r_type;

typedef struct {
	i32 imm;
	u8 rd;
	u8 opcode;
	u8 padding[2];
} rve_u_type;

typedef struct {
	i16 imm;
	u8 rs1;
	u8 funct_3;
	u8 rd;
	u8 opcode;
	u8 padding[2];
} rve_i_type;

typedef struct {
	i16 imm;
	u8 rs2;
	u8 rs1;
	u8 funct_3;
	u8 opcode;
	u8 padding;
} rve_s_type;

typedef struct {
	enum rve_instruction_type type;
	u16 instruction_index;
	u16 rs1Phy;
	u16 rs2Phy;
	u16 rdPhy;
	u8 data[8];
} rve_generic_inst;

void rve_print_inst(rve_generic_inst*);

typedef struct {
	u8 funct_3;
	u8 opcode;
	u8 funct_7;
	u8 execLatency;
	enum RVE_EXEC_PORTS validPorts;
	void (*exec)(rve_ctx* ctx, rve_generic_inst*);
	char name[8];
} rve_instruction;

#define RVE_NUM_SUPPORTED_INSTRUCTIONS 45

void rve_exec_lui(rve_generic_inst*);
void rve_exec_auipc(rve_generic_inst*);
void rve_exec_jal(rve_generic_inst*);
void rve_exec_jalr(rve_generic_inst*);
void rve_exec_beq(rve_generic_inst*);
void rve_exec_bne(rve_generic_inst*);
void rve_exec_blt(rve_generic_inst*);
void rve_exec_bge(rve_generic_inst*);
void rve_exec_bltu(rve_generic_inst*);
void rve_exec_bgeu(rve_generic_inst*);
void rve_exec_lb(rve_generic_inst*);
void rve_exec_lh(rve_generic_inst*);
void rve_exec_lw(rve_generic_inst*);
void rve_exec_lbu(rve_generic_inst*);
void rve_exec_lhu(rve_generic_inst*);
void rve_exec_sb(rve_generic_inst*);
void rve_exec_sh(rve_generic_inst*);
void rve_exec_sw(rve_generic_inst*);
void rve_exec_addi(rve_generic_inst*);
void rve_exec_slti(rve_generic_inst*);
void rve_exec_sltiu(rve_generic_inst*);
void rve_exec_xori(rve_generic_inst*);
void rve_exec_ori(rve_generic_inst*);
void rve_exec_andi(rve_generic_inst*);
void rve_exec_slli(rve_generic_inst*);
void rve_exec_srli(rve_generic_inst*);
void rve_exec_srai(rve_generic_inst*);
void rve_exec_add(rve_generic_inst*);
void rve_exec_sub(rve_generic_inst*);
void rve_exec_slt(rve_generic_inst*);
void rve_exec_sltu(rve_generic_inst*);
void rve_exec_xor(rve_generic_inst*);
void rve_exec_sll(rve_generic_inst*);
void rve_exec_srl(rve_generic_inst*);
void rve_exec_sra(rve_generic_inst*);
void rve_exec_or(rve_generic_inst*);
void rve_exec_and(rve_generic_inst*);
void rve_exec_mul(rve_generic_inst*);
void rve_exec_mulh(rve_generic_inst*);
void rve_exec_mulhsu(rve_generic_inst*);
void rve_exec_mulhu(rve_generic_inst*);
void rve_exec_div(rve_generic_inst*);
void rve_exec_divu(rve_generic_inst*);
void rve_exec_rem(rve_generic_inst*);
void rve_exec_remu(rve_generic_inst*);

enum RVE_IDX {
	RVE_BRANCH_INDEX = RVE_OPCODE_B,
	RVE_LOAD_INDEX = RVE_BRANCH_INDEX + 6,
	RVE_STORE_INDEX = RVE_LOAD_INDEX + 5,
	RVE_MATH_IMM_INDEX = RVE_STORE_INDEX + 3,
	RVE_MATH_INDEX = RVE_MATH_IMM_INDEX + 9,
	RVE_MATH_MUL_INDEX = RVE_MATH_INDEX + 10
};


static const rve_instruction rve_instructions[RVE_NUM_SUPPORTED_INSTRUCTIONS] = {
	{0b000, RVE_OPCODE_VAL_LUI, 0, 0, 0, rve_exec_lui, "lui"}, /// LUI
	{0b000, RVE_OPCODE_VAL_AUIPC, 0, 0, 0, rve_exec_auipc, "auipc"}, /// AUIPC
	{0b000, RVE_OPCODE_VAL_JAL, 0, 1, RVE_EXEC_PORT0 | RVE_EXEC_PORT6, rve_exec_jal, "jal"}, /// JAL
	{0b000, RVE_OPCODE_VAL_JALR, 0, 1, RVE_EXEC_PORT0 | RVE_EXEC_PORT6, rve_exec_jalr, "jalr"}, /// JALR
	{RVE_B_FUNCT3_BEQ, RVE_OPCODE_VAL_B, 1, RVE_EXEC_PORT0 | RVE_EXEC_PORT6, 0, rve_exec_beq, "beq"}, /// BEQ
	{RVE_B_FUNCT3_BNE, RVE_OPCODE_VAL_B, 1, RVE_EXEC_PORT0 | RVE_EXEC_PORT6, 0, rve_exec_bne, "bne"}, /// BNE
	{RVE_B_FUNCT3_BLT, RVE_OPCODE_VAL_B, 1, RVE_EXEC_PORT0 | RVE_EXEC_PORT6, 0, rve_exec_blt, "blt"}, /// BLT
	{RVE_B_FUNCT3_BGE, RVE_OPCODE_VAL_B, 1, RVE_EXEC_PORT0 | RVE_EXEC_PORT6, 0, rve_exec_bge, "bge"}, /// BGE
	{RVE_B_FUNCT3_BLTU, RVE_OPCODE_VAL_B, 1, RVE_EXEC_PORT0 | RVE_EXEC_PORT6, 0, rve_exec_bltu, "bltu"}, /// BLTU
	{RVE_B_FUNCT3_BGEU, RVE_OPCODE_VAL_B, 1, RVE_EXEC_PORT0 | RVE_EXEC_PORT6, 0, rve_exec_bgeu, "bgeu"}, /// BGEU
	{RVE_L_FUNCT3_LB, RVE_OPCODE_VAL_L, 1, RVE_EXEC_PORT2 | RVE_EXEC_PORT3, 0, rve_exec_lb, "lb"}, /// LB
	{RVE_L_FUNCT3_LH, RVE_OPCODE_VAL_L, 1, RVE_EXEC_PORT2 | RVE_EXEC_PORT3, 0, rve_exec_lh, "lh"}, /// LH
	{RVE_L_FUNCT3_LW, RVE_OPCODE_VAL_L, 1, RVE_EXEC_PORT2 | RVE_EXEC_PORT3, 0, rve_exec_lw, "lw"}, /// LW
	{RVE_L_FUNCT3_LBU, RVE_OPCODE_VAL_L, 1, RVE_EXEC_PORT2 | RVE_EXEC_PORT3, 0, rve_exec_lbu, "lbu"}, /// LBU
	{RVE_L_FUNCT3_LHU, RVE_OPCODE_VAL_L, 1, RVE_EXEC_PORT2 | RVE_EXEC_PORT3, 0, rve_exec_lhu, "lhu"}, /// LHU
	{RVE_S_FUNCT3_SB, RVE_OPCODE_VAL_S, 1, RVE_EXEC_PORT4 | RVE_EXEC_PORT9, 0, rve_exec_sb, "sb"}, /// SB
	{RVE_S_FUNCT3_SH, RVE_OPCODE_VAL_S, 1, RVE_EXEC_PORT4 | RVE_EXEC_PORT9, 0, rve_exec_sh, "sh"}, /// SH
	{RVE_S_FUNCT3_SW, RVE_OPCODE_VAL_S, 1, RVE_EXEC_PORT4 | RVE_EXEC_PORT9, 0, rve_exec_sw, "sw"}, /// SW
	{RVE_OP_MATH_FUNCT3_ADD, RVE_OPCODE_VAL_MATH_IMM, 0, 1, RVE_EXEC_PORT0 | RVE_EXEC_PORT1 | RVE_EXEC_PORT5 | RVE_EXEC_PORT6, rve_exec_addi, "addi"}, /// ADDI
	{RVE_OP_MATH_FUNCT3_SLT, RVE_OPCODE_VAL_MATH_IMM, 0, 1, RVE_EXEC_PORT0 | RVE_EXEC_PORT6, rve_exec_slti, "stli"}, /// SLTI
	{RVE_OP_MATH_FUNCT3_SLTU, RVE_OPCODE_VAL_MATH_IMM, 0, 1, RVE_EXEC_PORT0 | RVE_EXEC_PORT6, rve_exec_sltiu, "stliu"}, /// SLTIU
	{RVE_OP_MATH_FUNCT3_XOR, RVE_OPCODE_VAL_MATH_IMM, 0, 1, RVE_EXEC_PORT0 | RVE_EXEC_PORT1 | RVE_EXEC_PORT5 | RVE_EXEC_PORT6, rve_exec_xori, "xori"}, /// XORI
	{RVE_OP_MATH_FUNCT3_OR, RVE_OPCODE_VAL_MATH_IMM, 0, 1, RVE_EXEC_PORT0 | RVE_EXEC_PORT1 | RVE_EXEC_PORT5 | RVE_EXEC_PORT6, rve_exec_ori, "ori"}, /// ORI
	{RVE_OP_MATH_FUNCT3_AND, RVE_OPCODE_VAL_MATH_IMM, 0, 1, RVE_EXEC_PORT0 | RVE_EXEC_PORT1 | RVE_EXEC_PORT5 | RVE_EXEC_PORT6, rve_exec_andi, "andi"}, /// ANDI
	{RVE_OP_MATH_FUNCT3_SLL, RVE_OPCODE_VAL_MATH_IMM, 0, 1, RVE_EXEC_PORT0 | RVE_EXEC_PORT6, rve_exec_slli, "slli"}, /// SLLI
	{RVE_OP_MATH_FUNCT3_SRL, RVE_OPCODE_VAL_MATH_IMM, 0, 1, RVE_EXEC_PORT0 | RVE_EXEC_PORT6, rve_exec_srli, "srli"}, /// SRLI
	{RVE_OP_MATH_FUNCT3_SRA, RVE_OPCODE_VAL_MATH_IMM, 0b0100000, 1, RVE_EXEC_PORT0 | RVE_EXEC_PORT6, rve_exec_srai, "srai"}, /// SRAI
	{RVE_OP_MATH_FUNCT3_ADD, RVE_OPCODE_VAL_MATH, 0, 1, RVE_EXEC_PORT0 | RVE_EXEC_PORT1 | RVE_EXEC_PORT5 | RVE_EXEC_PORT6, rve_exec_add, "add"}, /// ADD
	{RVE_OP_MATH_FUNCT3_SUB, RVE_OPCODE_VAL_MATH, 0b0100000, 1, RVE_EXEC_PORT0 | RVE_EXEC_PORT1 | RVE_EXEC_PORT5 | RVE_EXEC_PORT6, rve_exec_sub, "sub"}, /// SUB
	{RVE_OP_MATH_FUNCT3_SLT, RVE_OPCODE_VAL_MATH, 0, 1, RVE_EXEC_PORT0 | RVE_EXEC_PORT6, rve_exec_slt, "slt"}, /// STL
	{RVE_OP_MATH_FUNCT3_SLTU, RVE_OPCODE_VAL_MATH, 0, 1, RVE_EXEC_PORT0 | RVE_EXEC_PORT6, rve_exec_sltu, "sltu"}, /// STLU
	{RVE_OP_MATH_FUNCT3_XOR, RVE_OPCODE_VAL_MATH, 0, 1, RVE_EXEC_PORT0 | RVE_EXEC_PORT1 | RVE_EXEC_PORT5 | RVE_EXEC_PORT6, rve_exec_xor, "xor"}, /// XOR
	{RVE_OP_MATH_FUNCT3_SLL, RVE_OPCODE_VAL_MATH, 0, 1, RVE_EXEC_PORT0 | RVE_EXEC_PORT6, rve_exec_sll, "sll"}, /// SLL
	{RVE_OP_MATH_FUNCT3_SRL, RVE_OPCODE_VAL_MATH, 0, 1, RVE_EXEC_PORT0 | RVE_EXEC_PORT6, rve_exec_srl, "srl"}, /// SRL
	{RVE_OP_MATH_FUNCT3_SRA, RVE_OPCODE_VAL_MATH, 0b0100000, 1, RVE_EXEC_PORT0 | RVE_EXEC_PORT6, rve_exec_sra, "sra"}, /// SRA
	{RVE_OP_MATH_FUNCT3_OR, RVE_OPCODE_VAL_MATH, 0, 1, RVE_EXEC_PORT0 | RVE_EXEC_PORT1 | RVE_EXEC_PORT5 | RVE_EXEC_PORT6, rve_exec_or, "or"}, /// OR
	{RVE_OP_MATH_FUNCT3_AND, RVE_OPCODE_VAL_MATH, 0, 1, RVE_EXEC_PORT0 | RVE_EXEC_PORT1 | RVE_EXEC_PORT5 | RVE_EXEC_PORT6, rve_exec_and, "and"}, /// AND
	/// FENCE
	/// FENCE.TSO
	/// PAUSE
	/// ECALL
	/// EBREAK
	{RVE_OP_MATH_FUNCT3_ADD, RVE_OPCODE_VAL_MATH, 0b1, 3, RVE_EXEC_PORT1, rve_exec_mul, "mul"}, /// MUL
	{RVE_OP_MATH_FUNCT3_SLL, RVE_OPCODE_VAL_MATH, 0b1, 3, RVE_EXEC_PORT1, rve_exec_mulh, "mulh"}, /// MULH
	{RVE_OP_MATH_FUNCT3_SLT, RVE_OPCODE_VAL_MATH, 0b1, 3, RVE_EXEC_PORT1, rve_exec_mulhsu, "mulhsu"}, /// MULHSU
	{RVE_OP_MATH_FUNCT3_SLTU, RVE_OPCODE_VAL_MATH, 0b1, 3, RVE_EXEC_PORT1, rve_exec_mulhu, "mulhu"}, /// MULHU
	{RVE_OP_MATH_FUNCT3_XOR, RVE_OPCODE_VAL_MATH, 0b1, 6, RVE_EXEC_PORT1, rve_exec_div, "div"}, /// DIV
	{RVE_OP_MATH_FUNCT3_SRL, RVE_OPCODE_VAL_MATH, 0b1, 6, RVE_EXEC_PORT1, rve_exec_divu, "divu"}, /// DIVU
	{RVE_OP_MATH_FUNCT3_OR, RVE_OPCODE_VAL_MATH, 0b1, 6, RVE_EXEC_PORT1, rve_exec_rem, "rem"}, /// REM
	{RVE_OP_MATH_FUNCT3_AND, RVE_OPCODE_VAL_MATH, 0b1, 6, RVE_EXEC_PORT1, rve_exec_remu, "remu"}, /// REMU
};
