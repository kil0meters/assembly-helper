#include "risc-v.h"

void rve_load_program(rve_ctx* ctx, u32* instruction_bytes, u32 len) {
	memcpy(ctx->memory, instruction_bytes, len * 4 <= ctx->memory_size ? len * 4 : ctx->memory_size);
}

rve_generic_inst rve_parse_and_schedule(rve_ctx* ctx, u32 instruction) {
	u8 opcode;
	enum RVE_OPCODES opcode_type;
	rve_generic_inst inst;
	opcode = instruction & 0b1111111;

	opcode_type = -1;
	for (u32 j = 0; j < RVE_NUM_SUPPORTED_OPCODES; j++) {
		if (opcode == RVE_OPCODE_VALS_ARR[j]) {
			opcode_type = j;
			break;
		}
	}
	if (opcode_type == -1) {
		printf("ERROR: Unknown opcode%x\n", opcode);
		exit(1);
	}
	switch (RVE_OPCODE_TYPES[opcode_type]) {
	case risc_v_r_type:
		inst = rve_parse_r_type(instruction, opcode_type);
		break;
	case risc_v_i_type:
		inst = rve_parse_i_type(instruction, opcode_type);
		break;
	case risc_v_s_type:
		inst = rve_parse_s_type(instruction, opcode_type);
		break;
	case risc_v_b_type:
		inst = rve_parse_b_type(instruction, opcode_type);
		break;
	case risc_v_u_type:
		inst = rve_parse_u_type(instruction, opcode_type);
		break;
	case risc_v_j_type:
		inst = rve_parse_j_type(instruction, opcode_type);
		break;
	default:
		printf("ERROR: Unknown instruction type %d\n", opcode_type);
		exit(1); /// NOTE: YAY!!!
	}

	rve_instructions[inst.instruction_index].exec(&inst);
	ctx->speculative_pc += 4;

	// rve_print_inst(&inst);
	// printf(": ");
	// rve_register_debug_print();
}

void rve_exec(rve_ctx* ctx, rve_generic_inst* inst) {

}

void rve_ctx_exec_program(rve_ctx* ctx) {
	u32 clock = 0;
	rve_generic_inst scheduleInsns[4];
	while((inst = rve_get_memory_u32(rve_register_get(RVE_PC_REGISTER)))) {
		scheduleInsns[0] = rve_parse_and_schedule(ctx);
		rve_parse_and_exec(inst);
	}
}
