#include <stdio.h>
#include "inst.h"
#include "exec.h"

#define RVE_MEMORY_SIZE (256 * 1024)

void rve_ctx_init(rve_ctx* ctx, u8* memory, u32 memory_size) {
	memset(ctx, 0, sizeof(rve_ctx));
	u32 i = 0;
	for (; i < 32; i++) {
		ctx->virtual_registers[i] = i;
		ctx->physical_registers[i].dependents = 1;
	}
	for (; i < PHYSICAL_REGISTER_COUNT; i++) {
		ctx->free_physical_registers[ctx->free_physical_register_count++] = i;
	}
	ctx->memory_size = memory_size;
	if (!memory) {
		memory = malloc(RVE_MEMORY_SIZE);
		ctx->owns_memory = B8_TRUE;
	}
	ctx->memory = memory;
}
void rvc_ctx_destroy(rve_ctx* ctx) {
	if (ctx->owns_memory) {
		free(ctx->memory);
	}
}

void rve_ctx_set_mem_word(rve_ctx* ctx, u32 byteIdx, u32 val) {
	if (byteIdx + 4 > ctx->memory_size) {
		ctx->hit_execution_error = B8_TRUE;
	} else {
		// Drillgon: I think the pointer cast thing is technically undefined behavior and I'm not sure how wasm will handle it. Doing memcpy just to be safe (wasm is little endian)
		memcpy(ctx->memory + byteIdx, &val, sizeof(u32));
	}
}
void rve_ctx_set_mem_hword(rve_ctx* ctx, u32 byteIdx, u16 val) {
	if (byteIdx + 2 > ctx->memory_size) {
		ctx->hit_execution_error = B8_TRUE;
	} else {
		memcpy(ctx->memory + byteIdx, &val, sizeof(u16));
	}
}
void rve_ctx_set_mem_byte(rve_ctx* ctx, u32 byteIdx, u8 val) {
	if (byteIdx + 1 > ctx->memory_size) {
		ctx->hit_execution_error = B8_TRUE;
	} else {
		ctx->memory[byteIdx] = val;
	}
}
u32 rve_ctx_get_mem_word(rve_ctx* ctx, u32 byteIdx) {
	u32 val = 0;
	if (byteIdx + 4 > ctx->memory_size) {
		ctx->hit_execution_error = B8_TRUE;
	} else {
		memcpy(&val, ctx->memory + byteIdx, sizeof(u32));
	}
	return val;
}
u16 rve_ctx_get_mem_hword(rve_ctx* ctx, u32 byteIdx) {
	u16 val = 0;
	if (byteIdx + 4 > ctx->memory_size) {
		ctx->hit_execution_error = B8_TRUE;
	} else {
		memcpy(&val, ctx->memory + byteIdx, sizeof(u16));
	}
	return val;
}
u8 rve_ctx_get_mem_byte(rve_ctx* ctx, u32 byteIdx) {
	u8 val = 0;
	if (byteIdx + 4 > ctx->memory_size) {
		ctx->hit_execution_error = B8_TRUE;
	} else {
		val = ctx->memory[byteIdx];
	}
	return val;
}

void rve_ctx_register_debug_print(rve_ctx* ctx) {
	u32 v;
	for (u32 i = 0; i < 32; i++) {
		if (v = ctx->physical_registers[ctx->virtual_registers[i]].value) {
			printf("x%d: %d, ", i, v);
		}
	}
	printf("PC: %d\n", ctx->pc);
}

void rve_ctx_unreference_physical_register(rve_ctx* ctx, u32 phy) {
	if (phy == 0) {
		return;
	}
	if (--ctx->physical_registers[phy].dependents == 0) {
		ctx->free_physical_registers[ctx->free_physical_register_count++] = phy;
	}
}
u32 rve_ctx_assign_physical_to_virtual_register(rve_ctx* ctx, u32 virtualReg) {
	if (virtualReg == 0) {
		return 0;
	}
	u32 phy = ctx->free_physical_registers[--ctx->free_physical_register_count];
	rve_ctx_unreference_physical_register(ctx, ctx->virtual_registers[virtualReg]);
	ctx->virtual_registers[virtualReg] = phy;
	ctx->physical_registers[phy].dependents = 1;
	return phy;
}



void rve_exec_lui(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_u_type* u_type = (rve_u_type*)&inst->data;
	rve_register_set(u_type->rd, u_type->imm);
}

void rve_exec_auipc(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_u_type* u_type = (rve_u_type*)&inst->data;
	rve_register_set(u_type->rd, rve_register_get(RVE_PC_REGISTER) + u_type->imm);
}

void rve_exec_jal(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_u_type* u_type = (rve_u_type*)&inst->data;
	u32 curr_pc = ctx->pc;
	rve_register_set(u_type->rd, curr_pc + 4);
	rve_register_add(RVE_PC_REGISTER, u_type->imm - 4);
}

void rve_exec_jalr(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	rve_register_set(i_type->rd, rve_register_get(RVE_PC_REGISTER) + 4);
	rve_register_set(RVE_PC_REGISTER, rve_register_get(i_type->rs1) + i_type->imm - 4);
}

void rve_exec_beq(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_s_type* s_type = (rve_s_type*)&inst->data;
	if (rve_register_get(s_type->rs1) == rve_register_get(s_type->rs2)) {
		rve_register_add(RVE_PC_REGISTER, s_type->imm - 4);
	}
}

void rve_exec_bne(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_s_type* s_type = (rve_s_type*)&inst->data;
	if (rve_register_get(s_type->rs1) != rve_register_get(s_type->rs2)) {
		rve_register_add(RVE_PC_REGISTER, s_type->imm - 4);
	}
}

void rve_exec_blt(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_s_type* s_type = (rve_s_type*)&inst->data;
	if ((i32)rve_register_get(s_type->rs1) < (i32)rve_register_get(s_type->rs2)) {
		rve_register_add(RVE_PC_REGISTER, s_type->imm - 4);
	}
}

void rve_exec_bge(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_s_type* s_type = (rve_s_type*)&inst->data;
	if ((i32)rve_register_get(s_type->rs1) > (i32)rve_register_get(s_type->rs2)) {
		rve_register_add(RVE_PC_REGISTER, s_type->imm - 4);
	}
}

void rve_exec_bltu(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_s_type* s_type = (rve_s_type*)&inst->data;
	if (rve_register_get(s_type->rs1) < rve_register_get(s_type->rs2)) {
		rve_register_add(RVE_PC_REGISTER, s_type->imm - 4);
	}
}

void rve_exec_bgeu(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_s_type* s_type = (rve_s_type*)&inst->data;
	if (rve_register_get(s_type->rs1) > rve_register_get(s_type->rs2)) {
		rve_register_add(RVE_PC_REGISTER, s_type->imm - 4);
	}
}

void rve_exec_lb(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	u32 address = rve_register_get(i_type->rs1) + i_type->imm;
	if (address > RVE_VIRTUAL_MEMORY_SIZE) {
		return;
	}
	u8 data = rve_memory[address];
	rve_register_set(i_type->rd, RVE_SIGN_EXTEND(data, 7));
}

void rve_exec_lh(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	u32 address = rve_register_get(i_type->rs1) + i_type->imm;
	if (address > RVE_VIRTUAL_MEMORY_SIZE) {
		return;
	}
	u16 data = (*(u16*)&(rve_memory[address]));
	rve_register_set(i_type->rd, RVE_SIGN_EXTEND(data, 15));
}

void rve_exec_lw(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	u32 address = rve_register_get(i_type->rs1) + i_type->imm;
	if (address > RVE_VIRTUAL_MEMORY_SIZE) {
		return;
	}
	u32 data = (*(u32*)&(rve_memory[address]));
	rve_register_set(i_type->rd, data);
}


void rve_exec_lbu(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	u32 address = rve_register_get(i_type->rs1) + i_type->imm;
	if (address > RVE_VIRTUAL_MEMORY_SIZE) {
		return;
	}
	u8 data = rve_memory[address];
	rve_register_set(i_type->rd, data);
}

void rve_exec_lhu(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	u32 address = rve_register_get(i_type->rs1) + i_type->imm;
	if (address > RVE_VIRTUAL_MEMORY_SIZE) {
		return;
	}
	u16 data = (*(u16*)&(rve_memory[address]));
	rve_register_set(i_type->rd, data);
}

void rve_exec_sb(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_s_type* s_type = (rve_s_type*)&inst->data;
	u32 address = rve_register_get(s_type->rs1) + s_type->imm;
	if (address > RVE_VIRTUAL_MEMORY_SIZE) {
		return;
	}
	rve_memory[address] = (u8)rve_register_get(s_type->rs2);
}

void rve_exec_sh(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_s_type* s_type = (rve_s_type*)&inst->data;
	u32 address = rve_register_get(s_type->rs1) + s_type->imm;
	if (address > RVE_VIRTUAL_MEMORY_SIZE) {
		return;
	}
	(*(u16*)(&(rve_memory[address]))) = (u16)rve_register_get(s_type->rs2);

}

void rve_exec_sw(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_s_type* s_type = (rve_s_type*)&inst->data;
	u32 address = rve_register_get(s_type->rs1) + s_type->imm;
	if (address > RVE_VIRTUAL_MEMORY_SIZE) {
		return;
	}
	(*(u32*)(&(rve_memory[address]))) = rve_register_get(s_type->rs2);
}

void rve_exec_addi(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	rve_register_set(i_type->rd, rve_register_get(i_type->rs1) + i_type->imm);
}

void rve_exec_slti(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	rve_register_set(i_type->rd, ((i32)rve_register_get(i_type->rs1) < i_type->imm));
}

void rve_exec_sltiu(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	rve_register_set(i_type->rd, rve_register_get(i_type->rs1) < (u32)i_type->imm);
}

void rve_exec_xori(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	rve_register_set(i_type->rd, rve_register_get(i_type->rs1) ^ i_type->imm);
}

void rve_exec_ori(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	rve_register_set(i_type->rd, rve_register_get(i_type->rs1) | i_type->imm);
}

void rve_exec_andi(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	rve_register_set(i_type->rd, rve_register_get(i_type->rs1) & i_type->imm);
}

void rve_exec_slli(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	rve_register_set(i_type->rd, rve_register_get(i_type->rs1) << (i_type->imm & 0b11111));
}

void rve_exec_srli(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	rve_register_set(i_type->rd, rve_register_get(i_type->rs1) >> (i_type->imm & 0b11111));
}

void rve_exec_srai(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	rve_register_set(i_type->rd, (i32)rve_register_get(i_type->rs1) >> (i_type->imm & 0b11111));

}

void rve_exec_add(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, rve_register_get(r_type->rs1) + rve_register_get(r_type->rs2));
}

void rve_exec_sub(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, rve_register_get(r_type->rs1) - rve_register_get(r_type->rs2));
}

void rve_exec_slt(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, ((i32)rve_register_get(r_type->rs1) < (i32)rve_register_get(r_type->rs2)));
}

void rve_exec_sltu(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, (rve_register_get(r_type->rs1) < rve_register_get(r_type->rs2)));
}

void rve_exec_xor(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, (rve_register_get(r_type->rs1) ^ rve_register_get(r_type->rs2)));
}

void rve_exec_sll(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, rve_register_get(r_type->rs1) << rve_register_get(r_type->rs2));
}

void rve_exec_srl(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, rve_register_get(r_type->rs1) >> rve_register_get(r_type->rs2));
}

void rve_exec_sra(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, (i32)rve_register_get(r_type->rs1) >> (i32)rve_register_get(r_type->rs2));
}

void rve_exec_or(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, rve_register_get(r_type->rs1) | rve_register_get(r_type->rs2));
}

void rve_exec_and(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, rve_register_get(r_type->rs1) & rve_register_get(r_type->rs2));
}

void rve_exec_mul(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, (i32)rve_register_get(r_type->rs1) * (i32)rve_register_get(r_type->rs2));
}

void rve_exec_mulh(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, ((i64)rve_register_get(r_type->rs1) * (i64)rve_register_get(r_type->rs2)) >> 32);
}

void rve_exec_mulhu(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, ((u64)rve_register_get(r_type->rs1) * (u64)rve_register_get(r_type->rs2)) >> 32);
}

void rve_exec_mulhsu(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, ((i64)rve_register_get(r_type->rs1) * (u64)rve_register_get(r_type->rs2)) >> 32);
}

void rve_exec_div(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, ((i32)rve_register_get(r_type->rs1) / (i32)rve_register_get(r_type->rs2)));
}

void rve_exec_divu(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, (rve_register_get(r_type->rs1) / rve_register_get(r_type->rs2)));
}

void rve_exec_rem(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, ((i32)rve_register_get(r_type->rs1) % (i32)rve_register_get(r_type->rs2)));
}

void rve_exec_remu(rve_ctx* ctx, rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	rve_register_set(r_type->rd, (rve_register_get(r_type->rs1) % rve_register_get(r_type->rs2)));
}
