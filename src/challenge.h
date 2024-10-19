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

#define NUM_CHALLENGES 4
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
        .for_lesson_title = "Assembly Tutorial",
        .challenge_index = 1,
        .slug = "/1",

        .title = "Intro To Assembly - Memory",
        .description = QUOTE(
            <p>
            	Now that you know how to set and add, its important to know how to interact
            	with memory. Our simulated CPU has 256kb of memory. All program code starts at
            	adrress 0 so you'll need to use an address high enough to not override the program.
            </p>

            <p>
                There are two necessary instructions for memory access, load and store. All memory
                instructions need to know the length of data theyre working with (b for byte, h for a half word or 2 byte, and w for a work or 4 bytes).
                For example, to store a byte you use the sb instruction.
            </p>

            <pre>
                s[bhw]  rd, imm(rs1)<br>
                l[bhw]  rd, imm(rs1)<br>
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
    // ===== LOOP UNROLLING =====
    {
        .id = 2,
        .for_lesson_title = "Loop Unrolling",
        .challenge_index = 0,
        .slug = "/0",

        .title = "Baby Steps",
        .description = "Here a loop. Make it faster.",
        .starter_code = "",
    },
    {
        .id = 3,
        .for_lesson_title = "Loop Unrolling",
        .challenge_index = 1,
        .slug = "/1",

        .title = "Teenage Steps",
        .description = "Here a loop. Make it faster. Again",
        .starter_code = "",
    },

    // ===== PIPELINING =====
    {
        .id = 4,
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
