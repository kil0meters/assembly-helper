#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "risc-v-const.h"
#include "register.h"
#include "parse.h"
#include "exec.h"


void rve_init();
void rve_load_program(u32*, u32);
void rve_parse_and_exec(u32);
void rve_exec_program();
