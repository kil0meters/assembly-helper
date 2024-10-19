#include <emscripten.h>

#include "util.h"
#include "lesson.h"
#include "components/base.h"
#include "components/lesson_overview.h"
#include "components/lesson_list.h"

char render_buffer[HTML_BUFFER_SIZE];

EM_JS(void, populate_selector_with_html, (const char *selector, const char *innerHTML), {
    let element = document.querySelector(UTF8ToString(selector));
    element.innerHTML = UTF8ToString(innerHTML);
});

EM_JS(void, set_element_classes, (const char *selector, const char *className), {
    let element = document.querySelector(UTF8ToString(selector));
    element.className = UTF8ToString(className);
});

EMSCRIPTEN_KEEPALIVE
void render_on_route() {
    char *route = (char*)EM_ASM_PTR({
      return stringToNewUTF8(window.location.hash);
    });
    jlog(route);

    bool found = false;
    set_element_classes("#sidebar", "closed");

    if (strncmp(route, "#/", 3) == 0 || strncmp(route, "#", 2) == 0 || strncmp(route, "", 1) == 0) {
        lesson_list_template(render_buffer, NUM_LESSONS, lessons);
        populate_selector_with_html("#content", render_buffer);
        found = true;
    } else {
        for (int i = 0; i < NUM_LESSONS; i++) {
            if (strncmp(route, lessons[i].slug, strlen(lessons[i].slug)) == 0) {
                lesson_list_template(render_buffer, NUM_LESSONS, lessons);
                populate_selector_with_html("#content", render_buffer);

                lesson_overview_template(render_buffer, lessons[i]);
                populate_selector_with_html("#sidebar", render_buffer);
                set_element_classes("#sidebar", "open");

                found = true;
                break;
            }
        }
    }

    if (!found) {
        jlog("404");
        snprintf(render_buffer, HTML_BUFFER_SIZE, QUOTE(<h1>404</h1><a href="#/">back to home</a>));
        populate_selector_with_html("#content", render_buffer);
    }

    free(route);
}

// Initialize function
EMSCRIPTEN_KEEPALIVE void init() {
    read_lesson_progress();

    emscripten_run_script("window.addEventListener('hashchange', () => Module._render_on_route())");
    emscripten_run_script("window.addEventListener('load', () => Module._render_on_route())");
    emscripten_run_script("Module._render_on_route()");
}
