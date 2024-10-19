#pragma once
#include "base.h"

const char *COMPLETION_STATUS_SUCCESS = QUOTE(
    <div class="completion-success">
        <h2>You did it!</h2>
        <a href="%s/%d">Next challenge &rarr;</a>
    </div>
);

const char *COMPLETION_STATUS_SUCCESS_COMPLETE = QUOTE(
    <div class="completion-success">
        <h2>You did it!</h2>
        <span>You completed all of the challenges for this lesson! <a href="#">Try another?</a></span>
    </div>
);

const char *COMPLETION_STATUS_FAILURE = QUOTE(
    <div class="completion-failure">
        <h2>Failure</h2>
        <span>Your code does not work or is too slow sad face</span>
    </div>
);

void completion_status_template(char *out, bool status) {
    if (status) {
        int next_challenge = current_challenge->challenge_index + 1;
        int i = 0;
        for (; i < NUM_CHALLENGES; i++) {
            if (challenges[i].challenge_index == next_challenge
             && strncmp(challenges[i].for_lesson_title, current_challenge->for_lesson_title, strlen(current_challenge->for_lesson_title)) == 0) {
                break;
            }
        }

        if (i == NUM_CHALLENGES) {
            snprintf(out, HTML_BUFFER_SIZE, "%s", COMPLETION_STATUS_SUCCESS_COMPLETE);
        } else {
            snprintf(out, HTML_BUFFER_SIZE, COMPLETION_STATUS_SUCCESS, current_lesson->slug, next_challenge);
        }
    } else {
        snprintf(out, HTML_BUFFER_SIZE, "%s", COMPLETION_STATUS_FAILURE);
    }
}
