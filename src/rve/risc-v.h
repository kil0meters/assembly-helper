#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "risc-v-const.h"
#include "parse.h"
#include "exec.h"

void rve_load_program(rve_ctx*, u32*, u32);
rve_generic_inst rve_parse_and_schedule(rve_ctx*, u32);
void rve_ctx_exec_program(rve_ctx*);
