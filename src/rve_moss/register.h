#pragma once
#include "risc-v-const.h"
#include <stdio.h>

#define RVE_PC_REGISTER (32)

void rve_register_init();
void rve_register_set(u32, u32);
u32 rve_register_get(u32);
void rve_register_add(u32, u32);
void rve_register_debug_print();
