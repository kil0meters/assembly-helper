#include <emscripten.h>

#include "util.h"
#include "lesson.h"
#include "challenge.h"

Challenge *current_challenge = NULL;
Lesson *current_lesson = NULL;

#include "rve_moss/risc-v.h"
#include "components/base.h"
#include "components/lesson_overview.h"
#include "components/challenge_view.h"
#include "components/lesson_list.h"

char render_buffer[HTML_BUFFER_SIZE];

EMSCRIPTEN_KEEPALIVE
void render_on_route() {
    char *route = (char*)EM_ASM_PTR({
      return stringToNewUTF8(window.location.hash);
    });

    bool found = false;
    set_element_classes("#root", "closed");

    current_challenge = NULL;
    current_lesson = NULL;

    if (strncmp(route, "#/", 3) == 0 || strncmp(route, "#", 2) == 0 || strncmp(route, "", 1) == 0) {
        lesson_list_template(render_buffer, NUM_LESSONS, lessons);
        populate_selector_with_html("#content", render_buffer);
        found = true;
    } else {
        for (int i = 0; i < NUM_LESSONS; i++) {
            int lesson_slug_start_len = strlen(lessons[i].slug);
            if (strncmp(route, lessons[i].slug, lesson_slug_start_len - 1) == 0) {
                if (strlen(route) == lesson_slug_start_len) {
                    lesson_list_template(render_buffer, NUM_LESSONS, lessons);
                    populate_selector_with_html("#content", render_buffer);

                    lesson_overview_template(render_buffer, &lessons[i]);
                    populate_selector_with_html("#sidebar", render_buffer);
                    set_element_classes("#root", "open");

                    current_lesson = &lessons[i];
                    found = true;
                    break;
                } else {
                    for (int j = 0; j < NUM_CHALLENGES; j++) {
                        if (strncmp(route + lesson_slug_start_len, challenges[j].slug, strlen(challenges[j].slug)) == 0
                         && strncmp(challenges[j].for_lesson_title, lessons[i].title, strlen(lessons[i].title)) == 0) {
                            challenge_view_template(render_buffer, &lessons[i], &challenges[j]);
                            populate_selector_with_html("#content", render_buffer);

                            challenge_sidebar_template(render_buffer, &challenges[j]);
                            populate_selector_with_html("#sidebar", render_buffer);
                            set_element_classes("#root", "open");

                            render_editor(challenges[j].starter_code);

                            current_lesson = &lessons[j];
                            current_challenge = &challenges[j];
                            found = true;
                            break;
                        }
                    }
                }
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

EMSCRIPTEN_KEEPALIVE void toggle_sidebar() {
    char *classes = get_element_classes("#root");
    if (strncmp(classes, "open", 5) == 0) {
        set_element_classes("#root", "closed");
    } else {
        set_element_classes("#root", "open");
    }
    free(classes);
}

// Initialize function
EMSCRIPTEN_KEEPALIVE void init() {
    read_challenge_progress();
    rve_init();

    EM_ASM({
        window.addEventListener('hashchange', () => Module._render_on_route());
        window.addEventListener('load', () => Module._render_on_route());
        Module._render_on_route();
    });
}
