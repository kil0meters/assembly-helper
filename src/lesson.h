#pragma once

#include <stdio.h>
#include <string.h>

#include "util.h"

typedef struct {
    char *title;
    char *slug;
    char *description;
    int total_challenges;
    int completed_challenges;
} Lesson;

#define NUM_LESSONS 4
Lesson lessons[NUM_LESSONS] = {
    {
        .title = "Loop Unrolling",
        .description = "Loop unrolling is a technique whereby",
        .slug = "#/lessons/unrolling",
        .total_challenges = 4,
        .completed_challenges = 0
    },
    {
        .title = "Pipelining",
        .slug = "#/lessons/pipelining",
        .total_challenges = 4,
        .completed_challenges = 0
    },
    {
        .title = "Cache Coherency",
        .slug = "#/lessons/cache",
        .total_challenges = 4,
        .completed_challenges = 0
    },
    {
        .title = "Branch Prediction",
        .slug = "#/lessons/branches",
        .total_challenges = 4,
        .completed_challenges = 0
    },
};

#define LESSON_PROGRESS_KEY "lesson-progress"

void read_lesson_progress() {
    char *progress = local_storage_get(LESSON_PROGRESS_KEY);
    char *token = strtok(progress, ",");

    while (token != NULL) {
        int i;

        for (i = 0; i < NUM_LESSONS; i++)
            if (strncmp(lessons[i].title, token, strlen(lessons[i].title)) == 0)
                break;
        if (i == NUM_LESSONS) break;

        token = strtok(NULL, ",");

        if (token != NULL) lessons[i].completed_challenges = atoi(token);
        else break;

        token = strtok(NULL, ",");
    }

    free(progress);
}

void save_lesson_progress() {
    char buf[1024];
    char *cur = buf;
    const char *end = buf + sizeof(buf);
    for (int i = 0; i < NUM_LESSONS; i++) {
        cur += snprintf(cur, end - cur, "%s,%d,", lessons[i].title, lessons[i].completed_challenges);
    }

    local_storage_set(LESSON_PROGRESS_KEY, buf);
}
