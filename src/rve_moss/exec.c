#include "inst.h"
#include "register.h"
#include <stdio.h>


#define RVE_VIRTUAL_MEMORY_SIZE 256000
u8 rve_memory[RVE_VIRTUAL_MEMORY_SIZE];


void rve_set_memory_u32(u32 data, u32 i) {
	*((u32*)(&rve_memory[i])) = data;
}

u32 rve_get_memory_u32(u32 i) {
	return *((u32*)(&rve_memory[i]));
}


void rve_exec_lui(rve_generic_inst* inst) {
	rve_u_type* u_type = (rve_u_type*)&inst->data;
	rve_register_set(u_type->rd, u_type->imm);
}

void rve_exec_auipc(rve_generic_inst* inst) {
	rve_u_type* u_type = (rve_u_type*)&inst->data;
	rve_register_set(u_type->rd, rve_register_get(RVE_PC_REGISTER) + u_type->imm);
}

void rve_exec_jal(rve_generic_inst* inst) {
	rve_u_type* u_type = (rve_u_type*)&inst->data;
	u32 curr_pc = rve_register_get(RVE_PC_REGISTER);
	rve_register_set(u_type->rd, curr_pc + 4);
	rve_register_add(RVE_PC_REGISTER, u_type->imm - 4);
}

void rve_exec_jalr(rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	rve_register_set(i_type->rd, rve_register_get(RVE_PC_REGISTER) + 4);
	rve_register_set(RVE_PC_REGISTER, rve_register_get(i_type->rs1) + i_type->imm - 4);
}

void rve_exec_beq(rve_generic_inst* inst) {
	rve_s_type* s_type = (rve_s_type*)&inst->data;
	if (rve_register_get(s_type->rs1) == rve_register_get(s_type->rs2)) {
		rve_register_add(RVE_PC_REGISTER, s_type->imm - 4);
	}
}

void rve_exec_bne(rve_generic_inst* inst) {
	rve_s_type* s_type = (rve_s_type*)&inst->data;
	if (rve_register_get(s_type->rs1) != rve_register_get(s_type->rs2)) {
		rve_register_add(RVE_PC_REGISTER, s_type->imm - 4);
	}
}

void rve_exec_blt(rve_generic_inst* inst) {
	rve_s_type* s_type = (rve_s_type*)&inst->data;
	if ((i32)rve_register_get(s_type->rs1) < (i32)rve_register_get(s_type->rs2)) {
		rve_register_add(RVE_PC_REGISTER, s_type->imm - 4);
	}
}

void rve_exec_bge(rve_generic_inst* inst) {
	rve_s_type* s_type = (rve_s_type*)&inst->data;
	if ((i32)rve_register_get(s_type->rs1) > (i32)rve_register_get(s_type->rs2)) {
		rve_register_add(RVE_PC_REGISTER, s_type->imm - 4);
	}
}

void rve_exec_bltu(rve_generic_inst* inst) {
	rve_s_type* s_type = (rve_s_type*)&inst->data;
	if (rve_register_get(s_type->rs1) < rve_register_get(s_type->rs2)) {
		rve_register_add(RVE_PC_REGISTER, s_type->imm - 4);
	}
}

void rve_exec_bgeu(rve_generic_inst* inst) {
	rve_s_type* s_type = (rve_s_type*)&inst->data;
	if (rve_register_get(s_type->rs1) > rve_register_get(s_type->rs2)) {
		rve_register_add(RVE_PC_REGISTER, s_type->imm - 4);
	}
}

void rve_exec_lb(rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	u32 address = rve_register_get(i_type->rs1) + i_type->imm;
	if (address > RVE_VIRTUAL_MEMORY_SIZE) {
		return;
	}
	u8 data = rve_memory[address];
	rve_register_set(i_type->rd, RVE_SIGN_EXTEND(data, 7));
}

void rve_exec_lh(rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	u32 address = rve_register_get(i_type->rs1) + i_type->imm;
	if (address > RVE_VIRTUAL_MEMORY_SIZE) {
		return;
	}
	u16 data = (*(u16*)&(rve_memory[address]));
	rve_register_set(i_type->rd, RVE_SIGN_EXTEND(data, 15));
}

void rve_exec_lw(rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	u32 address = rve_register_get(i_type->rs1) + i_type->imm;
	if (address > RVE_VIRTUAL_MEMORY_SIZE) {
		return;
	}
	u32 data = (*(u32*)&(rve_memory[address]));
	rve_register_set(i_type->rd, data);
}


void rve_exec_lbu(rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	u32 address = rve_register_get(i_type->rs1) + i_type->imm;
	if (address > RVE_VIRTUAL_MEMORY_SIZE) {
		return;
	}
	u8 data = rve_memory[address];
	rve_register_set(i_type->rd, data);
}

void rve_exec_lhu(rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	u32 address = rve_register_get(i_type->rs1) + i_type->imm;
	if (address > RVE_VIRTUAL_MEMORY_SIZE) {
		return;
	}
	u16 data = (*(u16*)&(rve_memory[address]));
	rve_register_set(i_type->rd, data);
}

void rve_exec_sb(rve_generic_inst* inst) {
	rve_s_type* s_type = (rve_s_type*)&inst->data;
	u32 address = rve_register_get(s_type->rs1) + s_type->imm;
	if (address > RVE_VIRTUAL_MEMORY_SIZE) {
		return;
	}
	rve_memory[address] = (u8)rve_register_get(s_type->rs2);
}

void rve_exec_sh(rve_generic_inst* inst) {
	rve_s_type* s_type = (rve_s_type*)&inst->data;
	u32 address = rve_register_get(s_type->rs1) + s_type->imm;
	if (address > RVE_VIRTUAL_MEMORY_SIZE) {
		return;
	}
	(*(u16*)(&(rve_memory[address]))) = (u16)rve_register_get(s_type->rs2);

}

void rve_exec_sw(rve_generic_inst* inst) {
	rve_s_type* s_type = (rve_s_type*)&inst->data;
	u32 address = rve_register_get(s_type->rs1) + s_type->imm;
	if (address > RVE_VIRTUAL_MEMORY_SIZE) {
		return;
	}
	(*(u32*)(&(rve_memory[address]))) = rve_register_get(s_type->rs2);
}

void rve_exec_addi(rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	rve_register_set(i_type->rd, rve_register_get(i_type->rs1) + i_type->imm);
}

void rve_exec_slti(rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	rve_register_set(i_type->rd, ((i32)rve_register_get(i_type->rs1) < i_type->imm));
}

void rve_exec_sltiu(rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	rve_register_set(i_type->rd, rve_register_get(i_type->rs1) < (u32)i_type->imm);
}

void rve_exec_xori(rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	rve_register_set(i_type->rd, rve_register_get(i_type->rs1) ^ i_type->imm);
}

void rve_exec_ori(rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	rve_register_set(i_type->rd, rve_register_get(i_type->rs1) | i_type->imm);
}

void rve_exec_andi(rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	rve_register_set(i_type->rd, rve_register_get(i_type->rs1) & i_type->imm);
}

void rve_exec_slli(rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	rve_register_set(i_type->rd, rve_register_get(i_type->rs1) << (i_type->imm & 0b11111));
}

void rve_exec_srli(rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	rve_register_set(i_type->rd, rve_register_get(i_type->rs1) >> (i_type->imm & 0b11111));
}

void rve_exec_srai(rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	rve_register_set(i_type->rd, (i32)rve_register_get(i_type->rs1) >> (i_type->imm & 0b11111));

}

void rve_exec_add(rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, rve_register_get(r_type->rs1) + rve_register_get(r_type->rs2));
}

void rve_exec_sub(rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, rve_register_get(r_type->rs1) - rve_register_get(r_type->rs2));
}

void rve_exec_slt(rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, ((i32)rve_register_get(r_type->rs1) < (i32)rve_register_get(r_type->rs2)));
}

void rve_exec_sltu(rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, (rve_register_get(r_type->rs1) < rve_register_get(r_type->rs2)));
}

void rve_exec_xor(rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, (rve_register_get(r_type->rs1) ^ rve_register_get(r_type->rs2)));
}

void rve_exec_sll(rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, rve_register_get(r_type->rs1) << rve_register_get(r_type->rs2));
}

void rve_exec_srl(rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, rve_register_get(r_type->rs1) >> rve_register_get(r_type->rs2));
}

void rve_exec_sra(rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, (i32)rve_register_get(r_type->rs1) >> (i32)rve_register_get(r_type->rs2));
}

void rve_exec_or(rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, rve_register_get(r_type->rs1) | rve_register_get(r_type->rs2));
}

void rve_exec_and(rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, rve_register_get(r_type->rs1) & rve_register_get(r_type->rs2));
}

void rve_exec_mul(rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, (i32)rve_register_get(r_type->rs1) * (i32)rve_register_get(r_type->rs2));
}

void rve_exec_mulh(rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, ((i64)rve_register_get(r_type->rs1) * (i64)rve_register_get(r_type->rs2)) >> 32);
}

void rve_exec_mulhu(rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, ((u64)rve_register_get(r_type->rs1) * (u64)rve_register_get(r_type->rs2)) >> 32);
}

void rve_exec_mulhsu(rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, ((i64)rve_register_get(r_type->rs1) * (u64)rve_register_get(r_type->rs2)) >> 32);
}

void rve_exec_div(rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, ((i32)rve_register_get(r_type->rs1) / (i32)rve_register_get(r_type->rs2)));
}

void rve_exec_divu(rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, (rve_register_get(r_type->rs1) / rve_register_get(r_type->rs2)));
}

void rve_exec_rem(rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, ((i32)rve_register_get(r_type->rs1) % (i32)rve_register_get(r_type->rs2)));
}

void rve_exec_remu(rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, (rve_register_get(r_type->rs1) % rve_register_get(r_type->rs2)));
}
