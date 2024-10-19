#include "risc-v.h"


int main() {
	u32 bytes[] = {0x200113, 0xf00193, 0x110113, 0xfe311ee3};

	rve_init();

	rve_load_program(bytes, sizeof(bytes) / sizeof(bytes[0]));

	rve_exec_program();
}
