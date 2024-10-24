#include <emscripten.h>
#include <pthread.h>
#include <string.h>

#include "challenge.h"
#include "components/editor.h"
#include "emscripten/emscripten.h"
#include "globals.h"
#include "lesson.h"
#include "signals.h"
#include "util.h"

#include "components/base.h"
#include "components/challenge_view.h"
#include "components/lesson_list.h"
#include "components/lesson_overview.h"
#include "rve_moss/risc-v.h"

#include "components/display.h"

EMSCRIPTEN_KEEPALIVE
void render_on_route() {
    char* route = (char*)EM_ASM_PTR({ return stringToNewUTF8(window.location.hash); });
    bool found = false;

    set_element_classes("#root", "closed");

    // always reset runtime on route change
    runtime_reset(g_runtime);

    g_current_challenge = NULL;
    g_current_lesson = NULL;

    if (strncmp(route, "#/", 3) == 0 || strncmp(route, "#", 2) == 0 || strncmp(route, "", 1) == 0) {
        jlog((char*)"what");
        lesson_list_component("#content", NUM_LESSONS, lessons);
        found = true;
    } else {
        for (int i = 0; i < NUM_LESSONS; i++) {
            int lesson_slug_start_len = strlen(lessons[i].slug);
            if (strncmp(route, lessons[i].slug, lesson_slug_start_len - 1) == 0) {
                if (strlen(route) == lesson_slug_start_len) {
                    lesson_list_component("#content", NUM_LESSONS, lessons);
                    lesson_overview_component("#sidebar", &lessons[i]);
                    set_element_classes("#root", "open");

                    g_current_lesson = &lessons[i];
                    found = true;
                    break;
                } else {
                    for (int j = 0; j < NUM_CHALLENGES; j++) {
                        if (strncmp(route + lesson_slug_start_len, challenges[j].slug, strlen(challenges[j].slug)) ==
                                0 &&
                            strncmp(challenges[j].for_lesson_title, lessons[i].title, strlen(lessons[i].title)) == 0) {
                            challenge_view_component("#content", &lessons[i], &challenges[j]);
                            challenge_sidebar_component("#sidebar", &challenges[j]);
                            set_element_classes("#root", "open");

                            editor_component(challenges[j].starter_code);
                            runner_component("#execution-buddy");

                            g_current_lesson = &lessons[j];
                            g_current_challenge = &challenges[j];
                            found = true;
                            break;
                        }
                    }
                }
            }
        }
    }

    if (!found) {
        snprintf(
            g_render_buffer, HTML_BUFFER_SIZE, QUOTE(
            <div class="p-16">
                <h1>404</h1>
                <a href="#/">back to home</a>
            </div>
        ));
        populate_selector_with_html("#content", g_render_buffer);
    }

    free(route);
}

EMSCRIPTEN_KEEPALIVE void toggle_sidebar() {
    char* classes = get_element_classes("#root");
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

    g_runtime = runtime_create();

    job_init();
    editor_init();

    // start simulation thread, never joined
    pthread_t thread;
    pthread_create(&thread, NULL, exe_loop, NULL);

    EM_ASM({
        window.addEventListener('hashchange', () => Module["_render_on_route"]());
        window.addEventListener('load', () => Module["_render_on_route"]());
        Module["_render_on_route"]();
    });

    // inner event loop
    while (true) {
        pthread_mutex_lock(&g_current_job_mutex);
        bool* p_running = signal_get(g_current_job->running);
        if (*p_running) {
            emulator_registers_component("#emulator-registers");
            display_component("#emulator-screen");

            pthread_mutex_unlock(&g_current_job_mutex);
        } else if (g_current_job->stopped) {
            g_current_job->stopped = false;
            *p_running = false;

            pthread_mutex_unlock(&g_current_job_mutex);

            // we call signal_set outside of the mutex in order to prevent a deadlock.
            // this is thread-safe because we are not actually mutating the value.
            signal_set(g_current_job->running, p_running);
        } else {
            pthread_mutex_unlock(&g_current_job_mutex);
        }

        // bool status = check_clear_condition(g_current_challenge->clear_condition);
        // if (status) {
        //     g_current_challenge->complete = true;
        //     save_challenge_progress();
        // }

        // completion_status_component(buffer, status);

        // run every frame
        emscripten_sleep(16);
    }
}
