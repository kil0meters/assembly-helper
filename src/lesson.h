#pragma once

typedef struct {
    char *title;
    char *slug;
    char *description;
    int total_challenges;
} Lesson;

Lesson lessons[] = {
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
#define NUM_LESSONS (sizeof(lessons) / sizeof(Lesson))
