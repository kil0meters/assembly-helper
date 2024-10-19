#include "register.h"

u32 rve_registers[33];


void rve_register_init() {
	rve_registers[0] = 0;
	rve_registers[RVE_PC_REGISTER] = 0;
}

void rve_register_set(u32 i, u32 val) {
	if (i == 0) return;
	rve_registers[i] = val; /// i is already validated as in range
}

u32 rve_register_get(u32 i) {
	return rve_registers[i]; /// NOTE: i is validated as in range
}

void rve_register_add(u32 i, u32 val) {
	if (i == 0) return;
	rve_registers[i] += val;
}


void rve_register_debug_print() {
	u32 v;
	for (int i = 0; i < 32; i++) {
		if ((v = rve_register_get(i))) {
				printf("x%d: %d, ", i, v);
		}
	}
	printf("PC: %d\n", rve_register_get(RVE_PC_REGISTER));
}
