#include "base.h"

#include "../executor.h"
#include "../signals.h"
#include <stdio.h>

char* RUNNER_TEMPLATE =
    QUOTE(
    <div>
        <div id="error-viewer"></div>

        <span class="p-4 pt-8 pl-12 ">Instruction Count</span> <span id="instruction-count" class="p-4 pr-12 pt-8 cycle-value text-right">---</span>
        <span class="p-4 pl-12">Target</span> <span class="p-4 pr-12 cycle-value text-right">%llu cycles</span>
        <span class="p-4 pl-12 pb-8">Last Execution</span> <span id="last-execution-cycles" class="p-4 pr-12 pb-8 cycle-value text-right">--- cycles</span>

        %s
    </div>
);

void render_runner() {
    pthread_mutex_lock(&g_current_job_mutex);
    bool running = *(bool*)signal_get(g_current_job->running);
    pthread_mutex_unlock(&g_current_job_mutex);

    printf("running: %d\n", running);

    snprintf(g_render_buffer,
             HTML_BUFFER_SIZE,
             RUNNER_TEMPLATE,
             10LLU,
             running
             ? QUOTE(<button id="execute-button" class="stopped" onclick="Module._editor_execute()">stop</button>)
             : QUOTE(<button id="execute-button" onclick="Module._editor_execute()">run</button>));

    populate_selector_with_html("#execution-buddy", g_render_buffer);
}

void runner_component(char* target) { effect_create(g_runtime, &render_runner); }
