#include "arr.h"
#include "risc-v.h"
#include <stdio.h>

void inst_array_alloc(rve_inst_array* arr, u32 size) {
	arr->data = calloc(size, sizeof(rve_instruction));
	arr->alloc_size = size;
}

void inst_array_realloc(rve_inst_array* arr, u32 size) {
	arr->data = realloc(arr->data, size * sizeof(rve_instruction)); // NOTE: The best memory allocation function
	arr->alloc_size = size;
}

void inst_array_create(rve_inst_array* arr, u32 size) {
	inst_array_alloc(arr, size);
	arr->len = 0;
}


void copy_inst(rve_generic_inst* src, rve_generic_inst* dst) {
	dst->type = src->type;
	dst->instruction_index = src->instruction_index;
	memcpy(dst->data, src->data, 8);
}

void inst_array_push(rve_inst_array* arr, rve_generic_inst inst) {
	if (!arr->data) return;
	if (arr->len >= arr->alloc_size) {
		inst_array_realloc(arr, arr->alloc_size*2);
	}
	rve_generic_inst* e = &arr->data[arr->len++];
	copy_inst(&inst, e);
}

rve_generic_inst* inst_array_get(rve_inst_array* arr, u32 i) {
	if (i >= arr->len) {
		printf("ERROR: Index Out of Bounds\n");
		exit(1); /// NOTE: Hard fail on error yay!!
	}
	return &arr->data[i];
}
void inst_array_set(rve_inst_array* arr, u32 i, rve_generic_inst inst) {
	if (i >= arr->len) {
		printf("ERROR: Index Out of Bounds\n");
		exit(1); /// NOTE: See above for thought on this
	}
	copy_inst(&inst, &arr->data[i]);
}
