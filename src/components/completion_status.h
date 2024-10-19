#pragma once
#include "base.h"

const char *COMPLETION_STATUS_SUCCESS = QUOTE(
    <div class="completion-success">
        <h2>You did it!</h2>
        <a>Next challenge &rarr;</a>
    </div>
);

const char *COMPLETION_STATUS_FAILURE = QUOTE(
    <div class="completion-failure">
        <h2>Failure</h2>
        <span>Your code is too slow!</span>
    </div>
);

void completion_status_template(char *out, bool status) {
    if (status) {
        snprintf(out, COMPLETION_STATUS_SUCESS, HTML_BUFFER_SIZE, );
    } else {
        snprintf(out, COMPLETION_STATUS_SUCESS, HTML_BUFFER_SIZE, );
    }
}
