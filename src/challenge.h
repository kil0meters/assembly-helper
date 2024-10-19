#pragma once

#include <stdint.h>

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
} Challenge;

#define NUM_CHALLENGES 3
Challenge challenges[] = {
    // ===== LOOP UNROLLING =====
    {
        .for_lesson_title = "Loop Unrolling",
        .challenge_index = 0,
        .slug = "/0",

        .title = "Baby Steps",
        .description = "Here a loop. Make it faster.",
        .starter_code = "",
        .required_cycle_count = 1000000ULL,
    },
    {
        .for_lesson_title = "Loop Unrolling",
        .challenge_index = 1,
        .slug = "/1",

        .title = "Teenage Steps",
        .description = "Here a loop. Make it faster. Again",
        .starter_code = "",
        .required_cycle_count = 1000000ULL,
    },

    // ===== PIPELINING =====
    {
        .for_lesson_title = "Pipelining",
        .challenge_index = 0,
        .slug = "/0",

        .title = "MARIO",
        .description = "Mario, we need your help!",
        .starter_code = "",
        .required_cycle_count = 1000000ULL,
    }
};
