#pragma once
#include "base.h"

#include "challenge_list.h"

const char* LESSON_OVERVIEW_TEMPLATE = QUOTE(
<div class="lesson-overview">
    <div class="flex">
        <h1>%s</h1>
        <a href="#/" class="close-button"><div></div></a>
    </div>
    <p>%s</p>

    <div id="challenge-list">
    </div>

    <div class="centered mt-auto">
        <a class="gradient-button" href="#/lessons/tutorial/0"><span>Begin Activity<span></a>
    </div>
</div>
);

void lesson_overview_component(char* target, Lesson* lesson) {
    snprintf(g_render_buffer, HTML_BUFFER_SIZE, LESSON_OVERVIEW_TEMPLATE, lesson->title, lesson->description);
    populate_selector_with_html(target, g_render_buffer);

    challenge_list_component("#challenge-list", lesson);
}
