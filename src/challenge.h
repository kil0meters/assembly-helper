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

#define NUM_CHALLENGES 4
Challenge challenges[] = {
    // ===== TUTORIAL =====
    {
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
    // ===== LOOP UNROLLING =====
    {
        .for_lesson_title = "Loop Unrolling",
        .challenge_index = 0,
        .slug = "/0",

        .title = "Baby Steps",
        .description = "Here a loop. Make it faster.",
        .starter_code = "",
    },
    {
        .for_lesson_title = "Loop Unrolling",
        .challenge_index = 1,
        .slug = "/1",

        .title = "Teenage Steps",
        .description = "Here a loop. Make it faster. Again",
        .starter_code = "",
    },

    // ===== PIPELINING =====
    {
        .for_lesson_title = "Pipelining",
        .challenge_index = 0,
        .slug = "/0",

        .title = "MARIO",
        .description = "Mario, we need your help!",
        .starter_code = "",
    }
};
