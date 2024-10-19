#pragma once
#include "base.h"

const char *LESSON_OVERVIEW_TEMPLATE = QUOTE(
<div class="lesson-overview">
    <h1>%s</h1>
    <p>%s</p>
    <div class="centered mt-auto">
        <a class="gradient-button" href="#/lesson/unrolling/activity"><span>Begin Activity<span></a>
    </div>
</div>
);

void lesson_overview_template(char *out, char *title) {
    snprintf(out, HTML_BUFFER_SIZE, LESSON_OVERVIEW_TEMPLATE, title, "lorem ipsum");
}
