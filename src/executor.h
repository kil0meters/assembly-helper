#include <emscripten/emscripten.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>

#include "globals.h"
#include "rva/RISCVAssembler.h"
#include "rve_moss/risc-v.h"
#include "signals.h"

typedef struct Job {
    RISCVWord* binary;
    Signal running;
    bool stopped;
} Job;

// this mutex is to be locked whenever the assigned job is being read
pthread_mutex_t g_current_job_mutex;
Job* g_current_job;

void job_init() {
    g_current_job = malloc(sizeof(Job));
    bool* p_stopped = malloc(sizeof(bool));
    *p_stopped = false;

    g_current_job->running = signal_create(g_runtime, p_stopped);
}

void* exe_loop(void*) {
    while (1) {
        pthread_mutex_lock(&g_current_job_mutex);
        bool stopped = g_current_job->stopped;
        bool* running = signal_get(g_current_job->running);

        if (*running && !stopped) {
            uint32_t inst = rve_get_memory_u32(rve_register_get(RVE_PC_REGISTER));
            if (inst) {
                rve_parse_and_exec(inst);
            } else {
                stopped = true;
            }
        }
        pthread_mutex_unlock(&g_current_job_mutex);

        if (stopped)
            emscripten_sleep(16);
    }

    return NULL;
}
