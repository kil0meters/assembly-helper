#pragma once
#include "base.h"

#include "../challenge.h"

const char *CHALLENGE_LIST_TEMPLATE = QUOTE(
    <div class="card lesson-list">
        %s
    </div>
);

const char *CHALLENGE_LIST_ITEM_TEMPLATE = QUOTE(
    <a href="%s%s" class="challenge-list-item">
        <span>%s</span>
        <b><span>%s</span></b>
    </a>
);

void challenge_list_template(char *out, Lesson *lesson) {
    char buffer[HTML_BUFFER_SIZE];
    char *cur = buffer;
    const char *end = buffer + sizeof(buffer);

    for (int i = 0; i < NUM_CHALLENGES; i++) {
        if (strncmp(challenges[i].for_lesson_title, lesson->title, strlen(lesson->title)) == 0) {
            cur += snprintf(cur, end - cur, CHALLENGE_LIST_ITEM_TEMPLATE,
                lesson->slug, challenges[i].slug,
                challenges[i].title, challenges[i].complete ? "COMPLETE" : "INCOMPLETE");
        }
    }

    if (end != cur) {
        snprintf(out, HTML_BUFFER_SIZE, CHALLENGE_LIST_TEMPLATE, buffer);
    } else {
        snprintf(out, HTML_BUFFER_SIZE, "");
    }
}
