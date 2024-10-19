#pragma once

#include <stdlib.h>
#include "RISCVLib.h"
#include "../rve_moss/risc-v-const.h"

typedef U32 RISCVWord;
#define RVA_REG_PC 32
#define RVA_REG_INVALID 32


typedef struct {
	StrA label;
	U32 offset;
} RVALabelLocation;
typedef struct {
	StrA originalSrc;
	StrA src;
	RISCVWord* insns;
	U32 insnCount;
	U32 insnCapacity;
	RVALabelLocation* labels;
	U32 labelCount;
	U32 labelCapacity;
	RVALabelLocation* labelPatches;
	U32 labelPatchCount;
	U32 labelPatchCapacity;
	B8 errored;
} RVACtx;

void rva_push_word(RVACtx* result, RISCVWord word) {
	if (result->insnCount == result->insnCapacity) {
		result->insnCapacity *= 2;
		result->insns = (RISCVWord*)realloc(result->insns, result->insnCapacity * sizeof(RISCVWord));
	}
	result->insns[result->insnCount++] = word;
}
void rva_push_label(RVACtx* result, StrA labelName, U32 offset) {
	if (result->labelCount == result->labelCapacity) {
		result->labelCapacity *= 2;
		result->labels = (RVALabelLocation*)realloc(result->labels, result->labelCapacity * sizeof(RVALabelLocation));
	}
	result->labels[result->labelCount++] = (RVALabelLocation){ labelName, offset };
}
void rva_push_label_patch(RVACtx* result, StrA labelName, U32 offset) {
	if (result->labelPatchCount == result->labelPatchCapacity) {
		result->labelPatchCapacity *= 2;
		result->labelPatches = (RVALabelLocation*)realloc(result->labelPatches, result->labelPatchCapacity * sizeof(RVALabelLocation));
	}
	result->labelPatches[result->labelPatchCount++] = (RVALabelLocation){ labelName, offset };
}
RVALabelLocation* rva_find_label(RVACtx* ctx, StrA name) {
	for (U32 i = 0; i < ctx->labelCount; i++) {
		if (stra_eq(ctx->labels[i].label, name)) {
			return ctx->labels + i;
		}
	}
	return NULL;
}

void rva_err_explicit(RVACtx* ctx, StrA errMsg, const void* errPosPtr) {
	ctx->errored = B8_TRUE;
	U32 errPos = errPosPtr ? (const char*)errPosPtr - ctx->originalSrc.str : ctx->src.str - ctx->originalSrc.str;
	U32 lineCount = 0;
	for (U32 i = 0; i < errPos; lineCount += ctx->originalSrc.str[i++] == '\n')
    	snprintf(ERROR_BUF, sizeof(ERROR_BUF), "Line %d: %s\n", lineCount, errMsg.str);
}
void rva_err(RVACtx* ctx, StrA errMsg) {
	rva_err_explicit(ctx, errMsg, NULL);
}

B8 rva_check_for_no_signed_overflow(U32 toCheck, U32 bitLength) {
	I32 toCheckS = (I32)(toCheck) >> (I32)(bitLength - 1);
	return toCheckS == 0 || toCheckS == -1;
}

void rva_encode_r(RVACtx* result, enum RVE_OPCODE_VALS opcode, U32 f3, U32 f7, U32 rd, U32 rs1, U32 rs2) {
	rva_push_word(result, f7 << 25 | rs2 << 20 | rs1 << 15 | f3 << 12 | rd << 7 | opcode);
}
void rva_encode_i(RVACtx* result, enum RVE_OPCODE_VALS opcode, U32 f3, U32 rd, U32 rs1, U32 imm) {
	if (imm >= 1 << 12) {
		rva_err(result, StrALit("Encoding failed, immediate too large"));
	}
	rva_push_word(result, imm << 20 | rs1 << 15 | f3 << 12 | rd << 7 | opcode);
}
void rva_encode_s(RVACtx* result, enum RVE_OPCODE_VALS opcode, U32 f3, U32 rs1, U32 rs2, U32 imm) {
	if (imm >= 1 << 12) {
		rva_err(result, StrALit("Encoding failed, immediate too large"));
	}
	rva_push_word(result, imm << 25 & 0b1111111 << 25 | rs2 << 20 | rs1 << 15 | f3 << 12 | imm << 7 & 0b11111 << 7 | opcode);
}
void rva_patch_b(RVACtx* ctx, RISCVWord* word, U32 imm) {
	if (imm & 1) {
		rva_err(ctx, StrALit("Encoding failed, B immediate must be even"));
	}
	if (imm >= 1 << 13) {
		rva_err(ctx, StrALit("Encoding failed, immediate too large"));
	}
	*word |= imm << 19 & 1 << 31 | imm << 20 & 0b111111 << 25 | imm << 7 & 0b1111 << 8 | imm >> 4 & 1 << 7;
}
void rva_encode_b(RVACtx* result, enum RVE_OPCODE_VALS opcode, U32 f3, U32 rs1, U32 rs2, U32 imm) {
	rva_push_word(result, rs2 << 20 | rs1 << 15 | f3 << 12 | opcode);
	rva_patch_b(result, &result->insns[result->insnCount - 1], imm);
}
void rva_encode_u(RVACtx* result, enum RVE_OPCODE_VALS opcode, U32 rd, U32 imm) {
	if (imm & (1 << 12) - 1) {
		rva_err(result, StrALit("Encoding failed, immediate has invalid low 12 bit set"));
	}
	rva_push_word(result, imm | rd << 7 | opcode);
}
void rva_patch_j(RVACtx* ctx, RISCVWord* word, U32 imm) {
	if (imm & 1) {
		rva_err(ctx, StrALit("Encoding failed, J immediate must be even"));
	}
	if (imm >= 1 << 21) {
		rva_err(ctx, StrALit("Encoding failed, immediate too large"));
	}
	*word |= imm << 11 & 1 << 31 | imm << 20 & 0b1111111111 << 21 | imm << 9 & 1 << 20 | imm & 0b11111111 << 12;
}
void rva_encode_j(RVACtx* result, enum RVE_OPCODE_VALS opcode, U32 rd, U32 imm) {
	rva_push_word(result, rd << 7 | opcode);
	rva_patch_j(result, &result->insns[result->insnCount - 1], imm);
}

void rva_consume(RVACtx* ctx, StrA beginning) {
	srt_skip_whitespace(&ctx->src);
	if (!stra_consume(&ctx->src, beginning)) {
		rva_err(ctx, StrALit("Expected token here"));
	}
}
B8 rva_try_consume(RVACtx* ctx, StrA beginning) {
	srt_skip_whitespace(&ctx->src);
	return stra_consume(&ctx->src, beginning);
}
I32 rva_consume_i32(RVACtx* ctx) {
	srt_skip_whitespace(&ctx->src);
	B8 isNegative = B8_FALSE;
	if (ctx->src.length && ctx->src.str[0] == '-') {
		ctx->src.str++, ctx->src.length--;
		isNegative = B8_TRUE;
	}
	U32 parsed = 0;
	if (stra_consume_u32(&parsed, &ctx->src)) {
		return isNegative ? -(I32)(parsed) : (I32)(parsed);
	} else {
		rva_err(ctx, StrALit("Expected I32"));
	}
	return 0;
}
B8 rva_parse_register(U32* regResult, RVACtx* ctx) {
	U32 regNum = 0;
	B8 errored = B8_FALSE;
	srt_skip_whitespace(&ctx->src);
	if (stra_consume(&ctx->src, StrALit("x"))) {
		errored |= !stra_consume_u32(&regNum, &ctx->src);
	} else if (stra_consume(&ctx->src, StrALit("zero"))) {
		regNum = 0;
	} else if (stra_consume(&ctx->src, StrALit("ra"))) {
		regNum = 1;
	} else if (stra_consume(&ctx->src, StrALit("sp"))) {
		regNum = 2;
	} else if (stra_consume(&ctx->src, StrALit("gp"))) {
		regNum = 3;
	} else if (stra_consume(&ctx->src, StrALit("tp"))) {
		regNum = 4;
	} else if (stra_consume(&ctx->src, StrALit("t"))) {
		errored |= !stra_consume_u32(&regNum, &ctx->src);
		regNum =
			regNum <= 2 ? regNum + 5 :
			regNum <= 6 ? regNum + 28 :
			RVA_REG_INVALID;
	} else if (stra_consume(&ctx->src, StrALit("fp"))) {
		regNum = 8;
	} else if (stra_consume(&ctx->src, StrALit("s"))) {
		regNum =
			regNum <= 1 ? regNum + 8 :
			regNum <= 11 ? regNum + 18 :
			RVA_REG_INVALID;
	} else if (stra_consume(&ctx->src, StrALit("a"))) {
		regNum = regNum <= 7 ? regNum + 10 : RVA_REG_INVALID;
	} else if (stra_consume(&ctx->src, StrALit("pc"))) {
		regNum = RVA_REG_PC;
	} else {
		regNum = RVA_REG_INVALID;
	}

	if (regNum >= RVA_REG_INVALID || errored) {
		return B8_FALSE;
	}
	*regResult = regNum;
	return B8_TRUE;
}
U32 rva_consume_register(RVACtx* ctx) {
	U32 reg = 0;
	if (rva_parse_register(&reg, ctx)) {
		return reg;
	}
	rva_err(ctx, StrALit("Expected register"));
	return 0;
}
B8 rva_parse_identifier(StrA* result, StrA* src) {
	srt_skip_whitespace(src);
	result->str = src->str;
	result->length = 0;
	while (src->length && srt_is_alphadigit(src->str[0])) {
		src->str++, src->length--;
		result->length++;
	}
	return result->length != 0;
}
StrA rva_consume_identifier(RVACtx* ctx) {
	StrA iden = { 0 };
	if (rva_parse_identifier(&iden, &ctx->src)) {
		return iden;
	}
	rva_err(ctx, StrALit("Expected identifier"));
	return (StrA) { 0 };
}
void make_conditional_branch(RVACtx* ctx, enum RVE_B_FUNCT3 branchType) {
	U32 rs1 = rva_consume_register(ctx);
	rva_consume(ctx, StrALit(","));
	U32 rs2 = rva_consume_register(ctx);
	rva_consume(ctx, StrALit(","));
	StrA jmpLabel = rva_consume_identifier(ctx);
	U32 offset = 0;
	RVALabelLocation* loc;
	if ((loc = rva_find_label(ctx, jmpLabel))) {
		U32 bMask = (1 << 13) - 1;
		offset = loc->offset * 4 - ctx->insnCount * 4;
		if ((~offset & ~bMask) != 0) {
			rva_err(ctx, StrALit("Jump out of range"));
		}
		offset &= bMask;
	} else {
		rva_push_label_patch(ctx, jmpLabel, ctx->insnCount);
	}
	rva_encode_b(ctx, RVE_OPCODE_VAL_B, branchType, rs1, rs2, offset);
}
RISCVWord* rva_assemble(U32* insnCount, StrA inSrc) {
	RVACtx ctx = { 0 };
	ctx.src = ctx.originalSrc = inSrc;
	ctx.insnCapacity = 1;
	ctx.insns = (RISCVWord*)malloc(ctx.insnCapacity * sizeof(RISCVWord));
	ctx.labelCapacity = 1;
	ctx.labels = (RVALabelLocation*)malloc(ctx.labelCapacity * sizeof(RVALabelLocation));
	ctx.labelPatchCapacity = 1;
	ctx.labelPatches = (RVALabelLocation*)malloc(ctx.labelPatchCapacity * sizeof(RVALabelLocation));
	while (ctx.src.length) {
		if (rva_try_consume(&ctx, StrALit(";"))) {
			while (ctx.src.length && ctx.src.str[0] != '\n') {
				ctx.src.str++, ctx.src.length--;
			}
			if (ctx.src.length) {
				ctx.src.str++, ctx.src.length--;
			}
			continue;
		}
		if (rva_try_consume(&ctx, StrALit("."))) {
			StrA iden = { 0 };
			if (!rva_parse_identifier(&iden, &ctx.src)) {
				rva_err(&ctx, StrALit("Directive must have identifier"));
				continue;
			}
			if (stra_eq(iden, StrALit("align"))) {
				U32 alignment = rva_consume_i32(&ctx);
				if (alignment < 2 || alignment > 16) {
					rva_err(&ctx, StrALit("Invalid alignment"));
					continue;
				}
				U32 alignmentMask = (1 << (alignment >> 2)) - 1;
				while (ctx.insnCount & alignmentMask) {
					rva_push_word(&ctx, 0);
				}
			} else {
				rva_err(&ctx, StrALit("Unknown directive"));
			}
			continue;
		}
		StrA iden = { 0 };
		if (rva_parse_identifier(&iden, &ctx.src)) {
			srt_skip_whitespace(&ctx.src);

#define GET_RRI()\
	U32 rd = rva_consume_register(&ctx);\
	rva_consume(&ctx, StrALit(","));\
	U32 rs1 = rva_consume_register(&ctx);\
	rva_consume(&ctx, StrALit(","));\
	U32 imm = rva_consume_i32(&ctx)

#define GET_RRR()\
	U32 rd = rva_consume_register(&ctx);\
	rva_consume(&ctx, StrALit(","));\
	U32 rs1 = rva_consume_register(&ctx);\
	rva_consume(&ctx, StrALit(","));\
	U32 rs2 = rva_consume_register(&ctx)

#define DO_MATH_IMM_FUNC(name)\
	GET_RRI();\
	rva_encode_i(&ctx, RVE_OPCODE_VAL_MATH_IMM, RVE_OP_MATH_FUNCT3_##name, rd, rs1, imm)

#define DO_MATH_REG_FUNC(name, hasModifier)\
	GET_RRR();\
	rva_encode_r(&ctx, RVE_OPCODE_VAL_MATH, RVE_OP_MATH_FUNCT3_##name, hasModifier << 5, rd, rs1, rs2)

#define DO_MATH_M_FUNC(name)\
	GET_RRR();\
	rva_encode_r(&ctx, RVE_OPCODE_VAL_MATH_MUL, RVE_OP_MUL_FUNCT3_##name, 0b0000001, rd, rs1, rs2)

#define DO_LOAD(type)\
	GET_RRI();\
	rva_encode_i(&ctx, RVE_OPCODE_VAL_L, RVE_L_FUNCT3_L##type, rd, rs1, imm)

#define DO_STORE(type)\
	GET_RRI();\
	rva_encode_s(&ctx, RVE_OPCODE_VAL_S, RVE_L_FUNCT3_L##type, rd, rs1, imm)

			if (ctx.src.length && ctx.src.str[0] == ':') {
				ctx.src.str++, ctx.src.length--;
				if (rva_find_label(&ctx, iden)) {
					rva_err(&ctx, StrALit("Label already exists"));
				}
				rva_push_label(&ctx, iden, ctx.insnCount);
				for (U32 i = 0; i < ctx.labelPatchCount; i++) {
					if (!stra_eq(ctx.labelPatches[i].label, iden)) {
						continue;
					}
					RISCVWord* wordToChange = &ctx.insns[ctx.labelPatches[i].offset];
					enum RVE_OPCODE_VALS opcode = (enum RVE_OPCODE_VALS)(*wordToChange & 0b1111111);
					U32 labelPatch = ctx.insnCount - ctx.labelPatches[i].offset;
					opcode == RVE_OPCODE_VAL_B ? rva_patch_b(&ctx, wordToChange, labelPatch) : rva_patch_j(&ctx, wordToChange, labelPatch);
					ctx.labelPatches[i--] = ctx.labelPatches[--ctx.labelPatchCount];
				}
			} else if (stra_eq_no_case(iden, StrALit("addi"))) {
				DO_MATH_IMM_FUNC(ADD);
			} else if (stra_eq_no_case(iden, StrALit("nop"))) {
				rva_encode_i(&ctx, RVE_OPCODE_VAL_MATH_IMM, RVE_OP_MATH_FUNCT3_ADD, 0, 0, 0);
			} else if (stra_eq_no_case(iden, StrALit("slti"))) {
				DO_MATH_IMM_FUNC(SLT);
			} else if (stra_eq_no_case(iden, StrALit("sltiu"))) {
				DO_MATH_IMM_FUNC(SLTU);
			} else if (stra_eq_no_case(iden, StrALit("xori"))) {
				DO_MATH_IMM_FUNC(SLT);
			} else if (stra_eq_no_case(iden, StrALit("ori"))) {
				DO_MATH_IMM_FUNC(SLT);
			} else if (stra_eq_no_case(iden, StrALit("andi"))) {
				DO_MATH_IMM_FUNC(SLT);
			} else if (stra_eq_no_case(iden, StrALit("slli"))) {
				GET_RRI();
				ctx.errored |= imm > 31;
				rva_encode_i(&ctx, RVE_OPCODE_VAL_MATH_IMM, RVE_OP_MATH_FUNCT3_SLL, rd, rs1, imm);
			} else if (stra_eq_no_case(iden, StrALit("srli"))) {
				GET_RRI();
				ctx.errored |= imm > 31;
				rva_encode_i(&ctx, RVE_OPCODE_VAL_MATH_IMM, RVE_OP_MATH_FUNCT3_SRL, rd, rs1, imm);
			} else if (stra_eq_no_case(iden, StrALit("srai"))) {
				GET_RRI();
				ctx.errored |= imm > 31;
				imm |= 1 << 10;
				rva_encode_i(&ctx, RVE_OPCODE_VAL_MATH_IMM, RVE_OP_MATH_FUNCT3_SRL, rd, rs1, imm);
			} else if (stra_eq_no_case(iden, StrALit("add"))) {
				DO_MATH_REG_FUNC(ADD, 0);
			} else if (stra_eq_no_case(iden, StrALit("sub"))) {
				DO_MATH_REG_FUNC(SUB, 1);
			} else if (stra_eq_no_case(iden, StrALit("sll"))) {
				DO_MATH_REG_FUNC(SLL, 0);
			} else if (stra_eq_no_case(iden, StrALit("slt"))) {
				DO_MATH_REG_FUNC(SLT, 0);
			} else if (stra_eq_no_case(iden, StrALit("sltu"))) {
				DO_MATH_REG_FUNC(SLTU, 0);
			} else if (stra_eq_no_case(iden, StrALit("xor"))) {
				DO_MATH_REG_FUNC(XOR, 0);
			} else if (stra_eq_no_case(iden, StrALit("srl"))) {
				DO_MATH_REG_FUNC(SRL, 0);
			} else if (stra_eq_no_case(iden, StrALit("sra"))) {
				DO_MATH_REG_FUNC(SRA, 1);
			} else if (stra_eq_no_case(iden, StrALit("or"))) {
				DO_MATH_REG_FUNC(OR, 0);
			} else if (stra_eq_no_case(iden, StrALit("and"))) {
				DO_MATH_REG_FUNC(AND, 0);
			} else if (stra_eq_no_case(iden, StrALit("mulhsu"))) {
				DO_MATH_M_FUNC(MULHSU);
			} else if (stra_eq_no_case(iden, StrALit("mulhu"))) {
				DO_MATH_M_FUNC(MULHU);
			} else if (stra_eq_no_case(iden, StrALit("mulh"))) {
				DO_MATH_M_FUNC(MULH);
			} else if (stra_eq_no_case(iden, StrALit("mul"))) {
				DO_MATH_M_FUNC(MUL);
			} else if (stra_eq_no_case(iden, StrALit("divu"))) {
				DO_MATH_M_FUNC(DIVU);
			} else if (stra_eq_no_case(iden, StrALit("div"))) {
				DO_MATH_M_FUNC(DIV);
			} else if (stra_eq_no_case(iden, StrALit("remu"))) {
				DO_MATH_M_FUNC(REMU);
			} else if (stra_eq_no_case(iden, StrALit("rem"))) {
				DO_MATH_M_FUNC(REM);
			} else if (stra_eq_no_case(iden, StrALit("jal"))) {
				U32 linkReg = rva_consume_register(&ctx);
				rva_consume(&ctx, StrALit(","));
				StrA jmpLabel = rva_consume_identifier(&ctx);
				U32 offset = 0;
				RVALabelLocation* loc;
				if ((loc = rva_find_label(&ctx, jmpLabel))) {
					U32 jMask = (1 << 21) - 1;
					offset = loc->offset * 4 - ctx.insnCount * 4;
					if ((~offset & ~jMask) != 0) {
						rva_err(&ctx, StrALit("Jump out of range"));
					}
					offset &= jMask;
				} else {
					rva_push_label_patch(&ctx, jmpLabel, ctx.insnCount);
				}
				rva_encode_j(&ctx, RVE_OPCODE_VAL_JAL, linkReg, offset);
			} else if (stra_eq_no_case(iden, StrALit("j"))) {
				StrA jmpLabel = rva_consume_identifier(&ctx);
				U32 offset = 0;
				RVALabelLocation* loc;
				if ((loc = rva_find_label(&ctx, jmpLabel))) {
					U32 jMask = (1 << 21) - 1;
					offset = loc->offset * 4 - ctx.insnCount * 4;
					if ((~offset & ~jMask) != 0) {
						rva_err(&ctx, StrALit("Jump out of range"));
					}
					offset &= jMask;
				} else {
					rva_push_label_patch(&ctx, jmpLabel, ctx.insnCount);
				}
				rva_encode_j(&ctx, RVE_OPCODE_VAL_JAL, 0, offset);
			} else if (stra_eq_no_case(iden, StrALit("jalr"))) {
				GET_RRI();
				rva_encode_i(&ctx, RVE_OPCODE_VAL_JALR, 0, rd, rs1, imm);
			} else if (stra_eq_no_case(iden, StrALit("beq"))) {
				make_conditional_branch(&ctx, RVE_B_FUNCT3_BEQ);
			} else if (stra_eq_no_case(iden, StrALit("bne"))) {
				make_conditional_branch(&ctx, RVE_B_FUNCT3_BNE);
			} else if (stra_eq_no_case(iden, StrALit("blt"))) {
				make_conditional_branch(&ctx, RVE_B_FUNCT3_BLT);
			} else if (stra_eq_no_case(iden, StrALit("blge"))) {
				make_conditional_branch(&ctx, RVE_B_FUNCT3_BGE);
			} else if (stra_eq_no_case(iden, StrALit("bltu"))) {
				make_conditional_branch(&ctx, RVE_B_FUNCT3_BLTU);
			} else if (stra_eq_no_case(iden, StrALit("bgeu"))) {
				make_conditional_branch(&ctx, RVE_B_FUNCT3_BGEU);
			} else if (stra_eq_no_case(iden, StrALit("lb"))) {
				DO_LOAD(B);
			} else if (stra_eq_no_case(iden, StrALit("lh"))) {
				DO_LOAD(H);
			} else if (stra_eq_no_case(iden, StrALit("lw"))) {
				DO_LOAD(W);
			} else if (stra_eq_no_case(iden, StrALit("lbu"))) {
				DO_LOAD(BU);
			} else if (stra_eq_no_case(iden, StrALit("lhu"))) {
				DO_LOAD(HU);
			} else if (stra_eq_no_case(iden, StrALit("sb"))) {
				DO_STORE(B);
			} else if (stra_eq_no_case(iden, StrALit("sh"))) {
				DO_STORE(H);
			} else if (stra_eq_no_case(iden, StrALit("sw"))) {
				DO_STORE(W);
			} else if (stra_eq_no_case(iden, StrALit("lui"))) {
				U32 rd = rva_consume_register(&ctx);
				rva_consume(&ctx, StrALit(","));
				I32 imm = rva_consume_i32(&ctx);
				rva_encode_u(&ctx, RVE_OPCODE_VAL_LUI, rd, imm);
			} else if (stra_eq_no_case(iden, StrALit("auipc"))) {
				U32 rd = rva_consume_register(&ctx);
				rva_consume(&ctx, StrALit(","));
				I32 imm = rva_consume_i32(&ctx);
				rva_encode_u(&ctx, RVE_OPCODE_VAL_AUIPC, rd, imm);
			} else {
				rva_err(&ctx, StrALit("Unknown opcode"));
			}

#undef DO_LOAD
#undef DO_STORE
#undef DO_MATH_IMM_FUNC
#undef DO_MATH_REG_FUNC
#undef GET_RRI
#undef GET_RRR
		}
	}
	for (U32 i = 0; i < ctx.labelPatchCount; i++) {
		rva_err_explicit(&ctx, StrALit("Unresolved label"), ctx.labelPatches[i].label.str);
	}
	free(ctx.labels);
	free(ctx.labelPatches);
	if (ctx.errored) {
		rva_err(&ctx, StrALit("Errors detected"));
		return NULL;
	}
	*insnCount = ctx.insnCount;
	return ctx.insns;
}

uint32_t* rva_assemble_cstr(uint32_t* insnCount, const char* inSrc) {
	return rva_assemble(insnCount, (StrA) { inSrc, strlen(inSrc) });
}
