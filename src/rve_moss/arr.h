#pragma once
#include "risc-v-const.h"
#include "inst.h"


typedef struct {
	u32 alloc_size, len;
	rve_generic_inst* data;
} rve_inst_array;

void inst_array_create(rve_inst_array*, u32);
void inst_array_push(rve_inst_array*, rve_generic_inst);
rve_generic_inst* inst_array_get(rve_inst_array*, u32);
void inst_array_set(rve_inst_array*, u32, rve_generic_inst);
