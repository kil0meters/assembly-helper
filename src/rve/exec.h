#pragma once
#include <string.h>
#include "risc-v-const.h"

#define RVE_PHYSICAL_REGISTER_COUNT 256

#define RVE_EXEC_PORT_COUNT 10
enum RVE_EXEC_PORTS {
	RVE_EXEC_PORT0 = 1 << 0,
	RVE_EXEC_PORT1 = 1 << 1,
	RVE_EXEC_PORT2 = 1 << 2,
	RVE_EXEC_PORT3 = 1 << 3,
	RVE_EXEC_PORT4 = 1 << 4,
	RVE_EXEC_PORT5 = 1 << 5,
	RVE_EXEC_PORT6 = 1 << 6,
	RVE_EXEC_PORT7 = 1 << 7,
	RVE_EXEC_PORT8 = 1 << 8,
	RVE_EXEC_PORT9 = 1 << 9,
};

// Drillgon: Might want more than one of these emulators running at once, or multiple saved for serialization purposes
typedef struct {
	u32 value;
	u32 dependents; // basically reference counted
} rve_physical_register;
#define RVE_IN_FLIGHT_INSN_COUNT 256
typedef struct {
	rve_generic_inst insn;
	u32 next; // 0xFFFFFFFF if null
} rve_in_flight_insn;
typedef struct {
	u32 inFlightInsnIdx; // 0xFFFFFFFF if null
	u32 assignedInsnCount;
} rve_execution_port;
typedef struct {
	rve_execution_port execution_ports[RVE_EXEC_PORT_COUNT];
	rve_physical_register physical_registers[RVE_PHYSICAL_REGISTER_COUNT];
	u32 free_physical_registers[RVE_PHYSICAL_REGISTER_COUNT];
	u32 free_physical_register_count;
	u32 virtual_registers[32]; // indexes into physicalRegisters
	u32 pc;
	u32 speculative_pc;
	u32 memory_size;
	u8* memory;
	b8 owns_memory;
	b8 hit_execution_error;
} rve_ctx;

u32 rve_ctx_assign_physical_to_virtual_register(rve_ctx*, u32);