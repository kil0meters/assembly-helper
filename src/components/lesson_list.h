#pragma once
#include "base.h"

const char *LESSON_LIST_TEMPLATE = QUOTE(
<div class="page">
    <h1>Assembly Playground</h1>
    <p>Learn about low-level optimizations by writing <a href="https://en.wikipedia.org/wiki/RISC-V">RISC-V</a> assembly.</p>
    <div class="card lesson-list">
        %s
    </div>
</div>
);

const char *LESSON_LIST_ITEM_TEMPLATE = QUOTE(
<a href="%s"><span>%s</span> <span class="badge">%d/%d</span></a>
);

void lesson_list_template(char *out, int num_items, Lesson *items) {
    char buffer[HTML_BUFFER_SIZE];
    char *cur = buffer;
    const char *end = buffer + sizeof(buffer);

    for (int i = 0; i < num_items; i++) {
        cur += snprintf(cur, end - cur, LESSON_LIST_ITEM_TEMPLATE,
                        items[i].slug, items[i].title,
                        items[i].completed_challenges, items[i].total_challenges);
    }

    snprintf(out, HTML_BUFFER_SIZE, LESSON_LIST_TEMPLATE, buffer);
}
