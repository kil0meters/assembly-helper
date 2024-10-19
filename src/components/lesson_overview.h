#pragma once
#include "base.h"

const char *LESSON_OVERVIEW_TEMPLATE = QUOTE(
<div>
    <h1>%s</h1>
</div>
);

void lesson_overview_template(char *out, char *title) {
    snprintf(out, HTML_BUFFER_SIZE, LESSON_OVERVIEW_TEMPLATE, title);
}
