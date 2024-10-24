#pragma once

#include "../challenge.h"
#include "../globals.h"
#include "../signals.h"
#include "base.h"

const char* COMPLETION_STATUS_SUCCESS =
    QUOTE(
    <div class="completion-success">
        <h2>You did it!</h2>
        <a href="%s/%d">Next challenge &rarr;</a>
    </div>
);

const char* COMPLETION_STATUS_SUCCESS_COMPLETE =
    QUOTE(
    <div class="completion-success">
        <h2>You did it!</h2>
        <span>You completed all of the challenges for this lesson! <a href="#">Try another?</a></span>
    </div>
);

const char* COMPLETION_STATUS_FAILURE =
    QUOTE(
    <div class="completion-failure">
        <h2>Failure</h2>
        <span>Your code does not work or is too slow sad face</span>
    </div>
);

void completion_status_component(char* target, Signal status) {
    bool statusb = (bool)signal_get(status);

    if (statusb) {
        int next_challenge = g_current_challenge->challenge_index + 1;
        int i = 0;
        for (; i < NUM_CHALLENGES; i++) {
            if (challenges[i].challenge_index == next_challenge &&
                strncmp(challenges[i].for_lesson_title,
                        g_current_challenge->for_lesson_title,
                        strlen(g_current_challenge->for_lesson_title)) == 0) {
                break;
            }
        }

        if (i == NUM_CHALLENGES) {
            snprintf(g_render_buffer, HTML_BUFFER_SIZE, "%s", COMPLETION_STATUS_SUCCESS_COMPLETE);
        } else {
            snprintf(
                g_render_buffer, HTML_BUFFER_SIZE, COMPLETION_STATUS_SUCCESS, g_current_lesson->slug, next_challenge);
        }
    } else {
        snprintf(g_render_buffer, HTML_BUFFER_SIZE, "%s", COMPLETION_STATUS_FAILURE);
    }

    populate_selector_with_html(target, g_render_buffer);
}
