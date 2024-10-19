#pragma once
#include "risc-v-const.h"

#define RVE_VIRTUAL_MEMORY_SIZE 256000
extern u8 rve_memory[RVE_VIRTUAL_MEMORY_SIZE];

void rve_set_memory_u32(u32 data, u32 i);
u32 rve_get_memory_u32(u32 i);
