#include "parse.h"
#include "exec.h"

#define rve_inst_as_generic(x) *((rve_generic_inst*)(&(x)))

u16 rve_get_instruction_index_r(rve_r_type* inst, enum RVE_OPCODES op) {
	switch (op) {
		case RVE_OPCODE_MATH: {
			if (inst->funct_7 == 0b1) {
				return RVE_MATH_MUL_INDEX + inst->funct_3;
			}
			u16 base = RVE_MATH_INDEX;
			switch(inst->funct_3) {
				case RVE_OP_MATH_FUNCT3_ADD:
					if (inst->funct_7 == 0b0100000) {
						return base + 1;
					}
					return base;
				case RVE_OP_MATH_FUNCT3_SLT:
					return base + 2;
				case RVE_OP_MATH_FUNCT3_SLTU:
					return base + 3;
				case RVE_OP_MATH_FUNCT3_XOR:
					return base + 4;
				case RVE_OP_MATH_FUNCT3_OR:
					return base + 5;
				case RVE_OP_MATH_FUNCT3_AND:
					return base + 6;
				case RVE_OP_MATH_FUNCT3_SLL:
					return base + 7;
				case RVE_OP_MATH_FUNCT3_SRL:
					if (inst->funct_7 == 0b0100000) { /// SRAI
						base++;
					}
					return base + 8;
				}
			}
		default: return 0;
	}
}
u16 rve_get_instruction_index_i(rve_i_type* inst, enum RVE_OPCODES op) {
	switch (op) {
		case RVE_OPCODE_JALR:
			return (u16)op;
		case RVE_OPCODE_L:
			switch (inst->funct_3) {
				case RVE_L_FUNCT3_LB:
					return RVE_LOAD_INDEX + 0;
				case RVE_L_FUNCT3_LH:
					return RVE_LOAD_INDEX + 1;
				case RVE_L_FUNCT3_LW:
					return RVE_LOAD_INDEX + 2;
				case RVE_L_FUNCT3_LBU:
					return RVE_LOAD_INDEX + 3;
				case RVE_L_FUNCT3_LHU:
					return RVE_LOAD_INDEX + 4;
			}
		case RVE_OPCODE_MATH_IMM: {
			u16 base = RVE_MATH_IMM_INDEX;
			switch(inst->funct_3) {
				case RVE_OP_MATH_FUNCT3_ADD:
					return base;
				case RVE_OP_MATH_FUNCT3_SLT:
					return base + 1;
				case RVE_OP_MATH_FUNCT3_SLTU:
					return base + 2;
				case RVE_OP_MATH_FUNCT3_XOR:
					return base + 3;
				case RVE_OP_MATH_FUNCT3_OR:
					return base + 4;
				case RVE_OP_MATH_FUNCT3_AND:
					return base + 5;
				case RVE_OP_MATH_FUNCT3_SLL:
					inst->imm &= 0b11111;
					return base + 6;
				case RVE_OP_MATH_FUNCT3_SRL:
					if ((inst->imm & 0b111111100000) == 0b010000000000) { /// SRAI
						base++; /// HACK: holy shit a hack in a hackathon omg. anyways this is just so we return 1 more than the regular srl cause thats where sra is i think. this might be wrong idk
					}
					inst->imm &= 0b11111;
					return base + 7;
				}
			}
		default: return 0;
	}
}
u16 rve_get_instruction_index_s(rve_s_type* inst, enum RVE_OPCODES op) {
	switch (op) {
		case RVE_OPCODE_S:
			switch (inst->funct_3) {
				case RVE_S_FUNCT3_SB:
					return RVE_STORE_INDEX + 0;
				case RVE_S_FUNCT3_SH:
					return RVE_STORE_INDEX + 1;
				case RVE_S_FUNCT3_SW:
					return RVE_STORE_INDEX + 2;
			}
		default: return 0;
	}
}
u16 rve_get_instruction_index_b(rve_s_type* inst, enum RVE_OPCODES op) {
	switch (op) {
		case RVE_OPCODE_B:
			switch (inst->funct_3) {
				case RVE_B_FUNCT3_BEQ:
					return RVE_BRANCH_INDEX + 0;
				case RVE_B_FUNCT3_BNE:
					return RVE_BRANCH_INDEX + 1;
				case RVE_B_FUNCT3_BLT:
					return RVE_BRANCH_INDEX + 2;
				case RVE_B_FUNCT3_BGE:
					return RVE_BRANCH_INDEX + 3;
				case RVE_B_FUNCT3_BLTU:
					return RVE_BRANCH_INDEX + 4;
				case RVE_B_FUNCT3_BGEU:
					return RVE_BRANCH_INDEX + 5;
			}
		default: return 0;
	}
}
u16 rve_get_instruction_index_u(rve_u_type* inst, enum RVE_OPCODES op) {
	switch (op) {
		case RVE_OPCODE_LUI:
		case RVE_OPCODE_AUIPC:
		case RVE_OPCODE_JAL:
			return (u16)op;
		default: return 0;
	}
}
u16 rve_get_instruction_index_j(rve_u_type* inst, enum RVE_OPCODES op) {
	if (op == RVE_OPCODE_JAL) {
		return (u16)op;
	}
	printf("HUH\n");
	exit(1);
}

rve_generic_inst rve_parse_r_type(rve_ctx* ctx, u32 inst, enum RVE_OPCODES op) {
	rve_r_type out;
	out.funct_7 = ((inst >> 25) & 0b1111111);
	out.rs2 = ((inst >> 20) & 0b11111);
	out.rs1 = ((inst >> 15) & 0b11111);
	out.funct_3 = ((inst >> 12) & 0b111);
	out.rd = ((inst >> 7) & 0b11111);
	out.opcode = (inst & 0b1111111);
	rve_generic_inst o;
	memcpy(o.data, &out, 8);
	o.type = risc_v_r_type;
	o.instruction_index = rve_get_instruction_index_r(&out, op);
	return o;
}

rve_generic_inst rve_parse_i_type(rve_ctx* ctx, u32 inst, enum RVE_OPCODES op) {
	rve_i_type out;
	out.imm = ((inst >> 20) & 0b111111111111);
	out.rs1 = ((inst >> 15) & 0b11111);
	out.funct_3 = ((inst >> 12) & 0b111);
	out.rd = ((inst >> 7) & 0b11111);
	out.opcode = (inst & 0b1111111);
	out.imm = RVE_SIGN_EXTEND(out.imm, 11);
	rve_generic_inst o;
	memcpy(o.data, &out, 8);
	o.type = risc_v_i_type;
	o.instruction_index = rve_get_instruction_index_i(&out, op);
	return o;
}

rve_generic_inst rve_parse_s_type(rve_ctx* ctx, u32 inst, enum RVE_OPCODES op) {
	rve_s_type out;
	out.imm = ((inst >> 25) & 0b1111111) << 5;
	out.rs2 = ((inst >> 20) & 0b11111);
	out.rs1 = ((inst >> 15) & 0b11111);
	out.funct_3 = ((inst >> 12) & 0b111);
	out.imm |= ((inst >> 7) & 0b11111);
	out.opcode = (inst & 0b1111111);
	out.imm = RVE_SIGN_EXTEND(out.imm, 11);
	rve_generic_inst o;
	memcpy(o.data, &out, 8);
	o.type = risc_v_s_type;
	o.instruction_index = rve_get_instruction_index_s(&out, op);
	return o;
}

rve_generic_inst rve_parse_b_type(rve_ctx* ctx, u32 inst, enum RVE_OPCODES op) {
	rve_s_type out;
	out.imm = ((inst >> 25) & 0b0111111) << 5;
	out.imm |= ((inst >> 31) & 0b1) << 12;
	out.rs2 = ((inst >> 20) & 0b11111);
	out.rs1 = ((inst >> 15) & 0b11111);
	out.funct_3 = ((inst >> 12) & 0b111);
	out.imm |= ((inst >> 7) & 0b11110);
	out.imm |= ((inst >> 7) & 0b1) << 11;
	out.imm = RVE_SIGN_EXTEND(out.imm, 11);
	out.opcode = (inst & 0b1111111);
	rve_generic_inst o;
	memcpy(o.data, &out, 8);
	o.type = risc_v_b_type;
	o.instruction_index = rve_get_instruction_index_b(&out, op);
	return o;
}

rve_generic_inst rve_parse_u_type(rve_ctx* ctx, u32 inst, enum RVE_OPCODES op) {
	rve_u_type out;
	out.imm = (inst & 0b11111111111111111111000000000000);
	out.rd = ((inst >> 7) & 0b11111);
	out.opcode = (inst & 0b1111111);
	rve_generic_inst o;
	memcpy(o.data, &out, 8);
	o.type = risc_v_u_type;
	o.instruction_index = rve_get_instruction_index_u(&out, op);
	return o;
}

rve_generic_inst rve_parse_j_type(rve_ctx* ctx, u32 inst, enum RVE_OPCODES op) {
	rve_u_type out;
	out.imm = ((inst >> 31) & 0b1) << 20;
	out.imm |= ((inst >> 21) & 0b1111111111) << 1;
	out.imm |= ((inst >> 20) & 0b1) << 11;
	out.imm |= ((inst >> 12) & 0b11111111) << 12;
	out.imm = RVE_SIGN_EXTEND(out.imm, 19);
	out.rd = ((inst >> 7) & 0b11111);
	out.opcode = (inst & 0b1111111);
	rve_generic_inst o;
	memcpy(o.data, &out, 8);
	o.type = risc_v_u_type;
	o.instruction_index = rve_get_instruction_index_j(&out, op);
	o.rdPhy = rve_ctx_assign_physical_to_virtual_register(ctx, out.rd);
	return o;
}
