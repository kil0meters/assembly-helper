#pragma once

#include <stdio.h>
#include <string.h>

#include "util.h"

typedef struct {
    char *title;
    char *slug;
    char *description;
    int total_challenges;
} Lesson;

#define NUM_LESSONS 5
Lesson lessons[NUM_LESSONS] = {
    {
        .title = "Assembly Tutorial",
        .description = "Learn the basics of RISC-V assembly.",
        .slug = "#/lessons/tutorial",
    },
    {
        .title = "Loop Unrolling",
        .description = "Loop unrolling is a technique whereby",
        .slug = "#/lessons/unrolling",
    },
    {
        .title = "Pipelining",
        .slug = "#/lessons/pipelining",
    },
    {
        .title = "Cache Coherency",
        .slug = "#/lessons/cache",
    },
    {
        .title = "Branch Prediction",
        .slug = "#/lessons/branches",
    },
};
