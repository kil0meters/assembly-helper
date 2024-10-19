#pragma once
#include "base.h"

#include "challenge_list.h"

const char *LESSON_OVERVIEW_TEMPLATE = QUOTE(
<div class="lesson-overview">
    <div class="flex">
        <h1>%s</h1>
        <a href="#/" class="close-button"><div></div></a>
    </div>
    <p>%s</p>
    %s
    <div class="centered mt-auto">
        <a class="gradient-button" href="#/lessons/unrolling/0"><span>Begin Activity<span></a>
    </div>
</div>
);

void lesson_overview_template(char *out, Lesson *lesson) {
    char challenge_list[HTML_BUFFER_SIZE];
    challenge_list_template(challenge_list, lesson);

    snprintf(out, HTML_BUFFER_SIZE, LESSON_OVERVIEW_TEMPLATE, lesson->title, lesson->description, challenge_list);
}
