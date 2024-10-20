#pragma once

#include <stdint.h>
#include "components/base.h"
#include "rve_moss/register.h"
#include "rve_moss/exec.h"

typedef enum ConditionalType {
    REGISTER_CONDITIONAL,
    MEMORY_CONDITIONAL,
} ConditionalType;

typedef struct {
    int target;
    uint32_t value;
} RegisterConditional;

typedef struct {
    uint32_t start; // start address to check for
    uint32_t len;   // length of buffer to check for
    uint8_t *buffer;
} MemoryConditional;

typedef union {
    RegisterConditional reg;
    MemoryConditional mem;
} ConditionalUnion;

typedef struct {
    uint64_t required_cycle_count;
    ConditionalUnion cond;
    ConditionalType descrim;
} ClearCondition;

bool check_clear_condition(ClearCondition c) {
    switch (c.descrim) {
        case REGISTER_CONDITIONAL:
            return rve_register_get(c.cond.reg.target) == c.cond.reg.value;
        case MEMORY_CONDITIONAL:
            for (uint32_t i = 0; i < c.cond.mem.len; i++)
                if (rve_memory[c.cond.mem.start + i] != c.cond.mem.buffer[i])
                    return false;
            return true;
    }

    return false;
}

typedef struct {
    int id;
    char *for_lesson_title;
    int challenge_index;
    char *slug;

    char *title;
    char *description;
    char *starter_code;
    uint64_t required_cycle_count;

    // dynamically loaded in from localstorage
    char *code;
    bool complete;
    ClearCondition clear_condition;
} Challenge;

#define NUM_CHALLENGES 6
Challenge challenges[] = {
    // ===== TUTORIAL =====
    {
        .id = 1,
        .for_lesson_title = "Assembly Tutorial",
        .challenge_index = 0,
        .slug = "/0",

        .title = "Intro To Assembly - Adding",
        .description = QUOTE(
            <p>
                That scary block of numbers above you are the values of the registers
                present in our simulated CPU. Do not worry, all you need to know
                for now is that each register can hold a value.
            </p>

            <p>
                All code on this website is submitted in RISC-V assembly.
                Here is an exhaustive overview of the basic arithmetic
                operations provided by the RISC-V instruction set.
            </p>

            <pre>
                addi rd, rs1, imm<br>
                add rd, rs1, rs2<br>
                sub rd, rs1, rs2<br>
            </pre>

            <p>
                Can you set the <code>x2</code> register to <code>0x20</code>?
            </p>
        ),
        .starter_code = "; type some code here\n; try to set the x2 register to 0x20!\n",
        .clear_condition = {
            .descrim = REGISTER_CONDITIONAL,
            .required_cycle_count = 1000000ULL,
            .cond = { .reg = { 2, 0x20 } },
        }
    },
    {
        .id = 2,
        .for_lesson_title = "Assembly Tutorial",
        .challenge_index = 1,
        .slug = "/1",

        .title = "Intro To Assembly - Memory",
        .description = QUOTE(
            <p>
            	Now that you know how to do basic arithmetic, its important to know how to interact
            	with memory. Our simulated CPU has 256kb of memory. All program code starts at
            	adrress 0 so you will need to use an address high enough to not override the program.
            </p>

            <p>
                There are two necessary instructions for memory access, load and store. All memory
                instructions need to know the length of data theyre working with (b for byte, h for a half word or 2 byte, and w for a work or 4 bytes).
                For example, to store a byte you use the sb instruction.
            </p>

            <pre>
                s[bhw] rsi, rd, imm<br>
                l[bhw] rsi rd, imm<br>
            </pre>

            <p>
                Can you set address <code>0x1000</code> to <code>0x15</code>?
            </p>
        ),
        .starter_code = "addi x2, 0x15\n; Set a register to the address you want to write to and use the store instruction\n",
        .clear_condition = {
            .descrim = MEMORY_CONDITIONAL,
            .required_cycle_count = 1000000ULL,
            .cond = { .mem = { 0x1000, 0x4,  (u8*)"\x15\x0\x0\x0"} },
        }
    },
    {
        .id = 3,
        .for_lesson_title = "Assembly Tutorial",
        .challenge_index = 2,
        .slug = "/2",

        .title = "Intro To Assembly - Branching",
        .description = QUOTE(
            <p>
                Now you can do arithmetic and access memory, but a program that does the same thing everytime is boring.
                What you need is branches. Branches allow you to jump to a different part of memory if a condition is passed.
                Branches can be used like an if statement or a loop.
            </p>

            <p>
                The branch instruction need a condition. For example, beq is branch if equal. Branches can go forward or backwards in code.
            </p>

            <pre>
                beq rs1, rs2, label<br>
                bne rs1, rs2, label<br>
                blt rs1, rs2, label<br>
                bge rs1, rs2, label<br>
                bltu rs1, rs2, label<br>
                bgeu rs1, rs2, label<br>
            </pre>

            <p>
                Write some code to write the byte <code>0x1</code> to addresses <code>0x1000</code>-<code>0x1010</code>.
            </p>
        ),
        .starter_code = "addi x2, 0x1000\n",
        .clear_condition = {
            .descrim = MEMORY_CONDITIONAL,
            .required_cycle_count = 1000000ULL,
            .cond = { .mem = { 0x1000, 0x10,  (u8*)"\x1\x1\x1\x1\x1\x1\x1\x1\x1\x1\x1\x1\x1\x1\x1\x1"} },
        }
    },
    // ===== LOOP UNROLLING =====
    {
        .id = 4,
        .for_lesson_title = "Loop Unrolling",
        .challenge_index = 0,
        .slug = "/0",

        .title = "Baby Steps",
        .description = "Here a loop. Make it faster.",
        .starter_code = "",
    },
    {
        .id = 5,
        .for_lesson_title = "Loop Unrolling",
        .challenge_index = 1,
        .slug = "/1",

        .title = "Teenage Steps",
        .description = "Here a loop. Make it faster. Again",
        .starter_code = "",
    },

    // ===== BRANCH PREDICTION =====
    {

        .for_lesson_title = "Branch Prediction",
        .challenge_index = 0,
        .slug = "/0",

        .title = "Conway's Game of Life",
        .description = "<div id=\"conway\"></div>",
        .starter_code = " \
; Starting code \n\
lui x31, 0x1000\n\
lui x30, 0x2000\n\
addi x2, x0, 1\n\
sb x31, x2, 205\n\
sb x31, x2, 225\n\
sb x31, x2, 224\n\
sb x31, x2, 245\n\
sb x31, x2, 246\n\
\n\
addi x1, x0, 20 ; grid width \n\
add x2, x0, x0 \n\
addi x26, x0, 1 \n\
\n\
updateloop: \n\
\n\
; Update grid \n\
addi x3, x0, 0 ; x \n\
addi x4, x0, 0 ; y \n\
loopy: \n\
loopx: \n\
\n\
; count into x6 \n\
add x6, x0, x0 \n\
add x7, x0, x3 \n\
add x8, x0, x4 \n\
sub x7, x7, x26 ; inner x \n\
sub x8, x8, x26 ; inner y \n\
addi x9, x7, 3 ; boundx \n\
addi x10, x8, 3 ; boundy \n\
countloopy: \n\
countloopx: \n\
; Accumulate x6 \n\
bne x3, x7, notcenter \n\
beq x4, x8, innercontinue \n\
notcenter: \n\
bgeu x7, x1, innercontinue \n\
bgeu x8, x1, innercontinue \n\
\n\
mul x5, x4, x1 \n\
add x5, x5, x3 ; y * width + x \n\
add x5, x5, x31 ; current cell address \n\
lbu x18, x5, 0 \n\
add x6, x6, x18 \n\
\n\
innercontinue: \n\
addi x7, x7, 1\n\
bne x7, x9, countloopx\n\
addi x8, x8, 1\n\
bne x8, x10, countloopy\n\
\n\
mul x5, x4, x1\n\
add x5, x5, x3 ; y * width + x\n\
add x5, x5, x31 ; current cell address\n\
lbu x7, x5, 0 ; current value (1 if alive)\n\
\n\
; Compute new value based on x7 and x6\n\
bne x7, x0, livecell\n\
addi x21, x0, 3\n\
bne x6, x21, deadcell \n\
addi x7, x0, 1 \n\
j deadcell \n\
livecell: \n\
addi x21, x0, 2 \n\
beq x6, x21, cellok \n\
addi x21, x0, 3 \n\
beq x6, x21, cellok \n\
addi x7, x0, 1 \n\
cellok: \n\
deadcell: \n\
\n\
sub x5, x5, x31 \n\
add x5, x5, x30 ; Change address to result \n\
sb x5, x7, 0 \n\
\n\
addi x3, x3, 1 \n\
bne x3, x1, loopx \n\
addi x4, x4, 1 \n\
bne x4, x1, loopy \n\
\n\
add x29, x0, x31 ; dst \n\
add x28, x0, x30 ; src \n\
addi x27, x31, 400 ; end \n\
copyloop: \n\
lw x25, x28, 0 \n\
sw x29, x25, 0 \n\
addi x29, x29, 4 \n\
addi x28, x28, 4 \n\
bne x27, x29, copyloop \n\
\n\
j updateloop \n\
        ",
        // .starter_code = "addi x1, x0, 1\naddi x2, x0, 0x500\nsb x2, x1, 0x500",
    },

    // ===== PIPELINING =====
    {
        .id = 6,
        .for_lesson_title = "Pipelining",
        .challenge_index = 0,
        .slug = "/0",

        .title = "MARIO",
        .description = "Mario, we need your help!",
        .starter_code = "",
    }
};

#define CHALLENGE_PROGRESS_KEY "challenge-progress"
void read_challenge_progress() {
    char *progress = local_storage_get(CHALLENGE_PROGRESS_KEY);
    if (progress == NULL) return;

    char *token = strtok(progress, ",");

    while (token != NULL) {
        int i;

        for (i = 0; i < NUM_CHALLENGES; i++)
            if (challenges[i].id == atoi(token))
                break;
        if (i == NUM_CHALLENGES) break;

        token = strtok(NULL, ",");

        if (token != NULL) challenges[i].complete = atoi(token);
        else break;

        token = strtok(NULL, ",");
    }

    free(progress);
}

void save_challenge_progress() {
    char buf[1024];
    char *cur = buf;
    const char *end = buf + sizeof(buf);
    for (int i = 0; i < NUM_LESSONS; i++) {
        cur += snprintf(cur, end - cur, "%d,%d,", challenges[i].id, challenges[i].complete);
    }

    local_storage_set(CHALLENGE_PROGRESS_KEY, buf);
}
