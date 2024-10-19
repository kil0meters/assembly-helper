#pragma once
#include "base.h"

#include "challenge_list.h"

const char *LESSON_OVERVIEW_TEMPLATE = QUOTE(
<div class="lesson-overview">
    <h1>%s</h1>
    <p>%s</p>
    %s
    <div class="centered mt-auto">
        <a class="gradient-button" href="#/lesson/unrolling/activity"><span>Begin Activity<span></a>
    </div>
</div>
);

void lesson_overview_template(char *out, Lesson lesson) {
    char challenge_list[HTML_BUFFER_SIZE];
    challenge_list_template(challenge_list, lesson.title);

    snprintf(out, HTML_BUFFER_SIZE, LESSON_OVERVIEW_TEMPLATE, lesson.title, lesson.description, challenge_list);
}
