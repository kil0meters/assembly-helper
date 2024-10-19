#pragma once
#include "base.h"

const char *LESSON_LIST_TEMPLATE = QUOTE(
<div class="card lesson-list">
    %s
</div>
);

const char *LESSON_LIST_ITEM_TEMPLATE = QUOTE(
<a href="%s">%s</a>
);

typedef struct {
    char *title;
    char *slug;
} LessonListItem;

void lesson_list_template(char *out, int num_items, LessonListItem *items) {
    char tmp_buffer[HTML_BUFFER_SIZE] = "";
    char tmp_buffer2[HTML_BUFFER_SIZE] = "";

    for (int i = 0; i < num_items; i++) {
        snprintf(tmp_buffer, HTML_BUFFER_SIZE, LESSON_LIST_ITEM_TEMPLATE, items[i].slug, items[i].title);
        strncat(tmp_buffer2, tmp_buffer, HTML_BUFFER_SIZE);
    }

    snprintf(out, HTML_BUFFER_SIZE, LESSON_LIST_TEMPLATE, tmp_buffer2);
}
