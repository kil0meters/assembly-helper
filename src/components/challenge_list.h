#pragma once
#include "base.h"

#include "../challenge.h"

const char *CHALLENGE_LIST_TEMPLATE = QUOTE(
    <div class="card lesson-list">
        %s
    </div>
);

const char *CHALLENGE_LIST_ITEM_TEMPLATE = QUOTE(
    <span>%s</span>
);

void challenge_list_template(char *out, char *lesson_title) {
    char buffer[HTML_BUFFER_SIZE];
    char *cur = buffer;
    const char *end = buffer + sizeof(buffer);

    for (int i = 0; i < NUM_CHALLENGES; i++) {
        if (strncmp(challenges[i].for_lesson_title, lesson_title, strlen(lesson_title)) == 0) {
            cur += snprintf(cur, end - cur, CHALLENGE_LIST_ITEM_TEMPLATE, challenges[i].title);
        }
    }

    snprintf(out, HTML_BUFFER_SIZE, CHALLENGE_LIST_TEMPLATE, buffer);
}
