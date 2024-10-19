#include "inst.h"
#include <stdio.h>

void rve_print_inst_r_type(rve_generic_inst* inst) {
	rve_r_type* r_type = (rve_r_type*)&inst->data;
	printf("x%d, x%d, x%d", r_type->rd, r_type->rs1, r_type->rs2);
}

void rve_print_inst_i_type(rve_generic_inst* inst) {
	rve_i_type* i_type = (rve_i_type*)&inst->data;
	printf("x%d, x%d, %d", i_type->rd, i_type->rs1, i_type->imm);
}

void rve_print_inst_s_type(rve_generic_inst* inst) {
	rve_s_type* s_type = (rve_s_type*)&inst->data;
	printf("x%d, x%d(%d)", s_type->rs1, s_type->rs2, s_type->imm);
}

void rve_print_inst_b_type(rve_generic_inst* inst) {
	rve_s_type* s_type = (rve_s_type*)&inst->data;
	printf("x%d, x%d, %d", s_type->rs1, s_type->rs2, s_type->imm);

}

void rve_print_inst_u_type(rve_generic_inst* inst) {
	rve_u_type* u_type = (rve_u_type*)&inst->data;
	printf("x%d, %d", u_type->rd, u_type->imm);
}

void rve_print_inst(rve_generic_inst* inst) {
	printf("%s, ", rve_instructions[inst->instruction_index].name);
	switch (inst->type) {
		case risc_v_r_type:
			rve_print_inst_r_type(inst);
			break;
		case risc_v_i_type:
			rve_print_inst_i_type(inst);
			break;
		case risc_v_s_type:
			rve_print_inst_s_type(inst);
			break;
		case risc_v_b_type:
			rve_print_inst_b_type(inst);
			break;
		case risc_v_u_type:
		case risc_v_j_type:
			rve_print_inst_u_type(inst);
			break;
	}
}
