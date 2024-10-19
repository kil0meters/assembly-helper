#pragma once
#include "inst.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

rve_generic_inst rve_parse_r_type(u32 inst, enum RVE_OPCODES op);
rve_generic_inst rve_parse_i_type(u32 inst, enum RVE_OPCODES op);
rve_generic_inst rve_parse_s_type(u32 inst, enum RVE_OPCODES op);
rve_generic_inst rve_parse_b_type(u32 inst, enum RVE_OPCODES op);
rve_generic_inst rve_parse_u_type(u32 inst, enum RVE_OPCODES op);
rve_generic_inst rve_parse_j_type(u32 inst, enum RVE_OPCODES op);
