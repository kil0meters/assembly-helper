#include <emscripten.h>

#include "util.h"
#include "components/base.h"
#include "components/lesson_overview.h"
#include "components/lesson_list.h"

char render_buffer[HTML_BUFFER_SIZE];

EM_JS(void, populate_selector_with_html, (const char *selector, const char *innerHTML), {
    let element = document.querySelector(UTF8ToString(selector));
    element.innerHTML = UTF8ToString(innerHTML);
});


EMSCRIPTEN_KEEPALIVE
void render_on_route() {
    char *route = (char*)EM_ASM_PTR({
      return stringToNewUTF8(window.location.hash.substr(1));
    });

    jlog(route);

    if (strncmp(route, "/", 2) == 0 || strncmp(route, "", 1) == 0) {
        jlog("route 2");
        LessonListItem items[3] = {
            { .title = "Loop Unrolling", .slug = "#/lessons/unrolling" },
            { .title = "Pipelining", .slug = "#/lessons/pipelining" },
            { .title = "Baz", .slug = "#/lessons/baz" },
        };

        lesson_list_template(render_buffer, sizeof(items) / sizeof(LessonListItem), items);
    } else if (strncmp(route, "/lessons/unrolling", sizeof("/lessons/unrolling")) == 0) {
        jlog("route 2");
        lesson_overview_template(render_buffer, "Loop Unrolling");
    } else {
        snprintf(render_buffer, HTML_BUFFER_SIZE, "<h1>404</h1>");
    }

    populate_selector_with_html("#content", render_buffer);
    free(route);
}

// Initialize function
EMSCRIPTEN_KEEPALIVE void init() {
    emscripten_run_script("window.addEventListener('hashchange', () => Module._render_on_route())");
    emscripten_run_script("window.addEventListener('load', () => Module._render_on_route())");
    emscripten_run_script("Module._render_on_route()");
}
