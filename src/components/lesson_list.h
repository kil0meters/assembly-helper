#pragma once
#include "base.h"

#include "../challenge.h"
#include "../lesson.h"

const char* LESSON_LIST_TEMPLATE = " \
<div class=\"grid-container\"> \
    <div class=\"page\"> \
        <h1>Assembly Playground</h1> \
        <p>Learn about low-level optimizations by writing <a href=\"https://en.wikipedia.org/wiki/RISC-V\">RISC-V</a> assembly.</p> \
        <div class=\"lesson-grid\"> \
            %s \
        </div> \
    </div> \
    <div class=\"grid\"> \
        <pre class=\"placeholder\"> \
(•_•)<br>\
 <)  )╯ pick a problem,<br> \
 /  \\  cowboy<br></pre> \
    </div> \
</div>";

const char* LESSON_LIST_ITEM_TEMPLATE =
    QUOTE(
<a class="card" href="%s"><span class="inner-text">%s</span> <span class="badge">%d/%d</span></a>
);

void lesson_list_component(char* target, int num_items, Lesson* items) {
    char buffer[HTML_BUFFER_SIZE];
    char* cur = buffer;
    const char* end = buffer + sizeof(buffer);

    for (int i = 0; i < num_items; i++) {
        int completed_challenges = 0;
        int total_challenges = 0;

        for (int j = 0; j < NUM_CHALLENGES; j++) {
            if (strncmp(challenges[j].for_lesson_title, items[i].title, strlen(items[i].title)) == 0) {
                total_challenges += 1;
                if (challenges[j].complete) {
                    completed_challenges += 1;
                }
            }
        }

        cur += snprintf(cur,
                        end - cur,
                        LESSON_LIST_ITEM_TEMPLATE,
                        items[i].slug,
                        items[i].title,
                        completed_challenges,
                        total_challenges);
    }

    snprintf(g_render_buffer, HTML_BUFFER_SIZE, LESSON_LIST_TEMPLATE, buffer);
    populate_selector_with_html(target, g_render_buffer);
}
