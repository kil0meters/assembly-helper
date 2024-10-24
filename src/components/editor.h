#pragma once

#include <emscripten.h>
#include <emscripten/html5.h>
#include <pthread.h>
#include <string.h>

#include "base.h"
#include "challenge_view.h"
#include "completion_status.h"
#include "emscripten/em_asm.h"
#include "emulator_registers.h"

#include "../rva/RISCVAssembler.h"
#include "../rve_moss/risc-v.h"

const char* EDITOR_TEMPLATE =
    QUOTE(
    <div id="line-numbers"></div>
    <div contenteditable spellcheck="false" id="editor" oninput="Module['_editor_on_input'](stringToNewUTF8(document.getElementById('editor').innerText))"></div>
);

Signal g_editor_text;

void editor_init() { g_editor_text = signal_create(g_runtime, NULL); }

void update_line_numbers() {
    char render_buffer[HTML_BUFFER_SIZE];
    char* cur = render_buffer;
    char* end = render_buffer + sizeof(render_buffer);

    int line_number = 1;
    cur += snprintf(cur, end - cur, "<span>%d</span>", line_number++);
    char* c = signal_get(g_editor_text);
    while (*c != '\0') {
        if (*c == '\n') {
            cur += snprintf(cur, end - cur, "<span>%d</span>", line_number++);
        }
        c++;
    }

    populate_selector_with_html("#line-numbers", render_buffer);
}

uint32_t inst_count;
RISCVWord* rv_binary;

void try_assemble() {
    char* editor_text = signal_get(g_editor_text);
    jlog(editor_text);

    g_assembler_error_count = 0;
    rv_binary = rva_assemble(&inst_count, (StrA){.str = editor_text, .length = strlen(editor_text)});

    if (rv_binary != NULL) {
        char* cur = g_render_buffer;
        char* end = g_render_buffer + HTML_BUFFER_SIZE;
        for (int i = 0; i < inst_count; i++) {
            cur += snprintf(cur, end - cur, " %08x", rv_binary[i]);
        }

        puts(g_render_buffer);

        snprintf(g_render_buffer, HTML_BUFFER_SIZE, "%u", inst_count);
        populate_selector_with_html("#instruction-count", g_render_buffer);
        populate_selector_with_html("#error-viewer", "");
    } else {
        char* cur = g_render_buffer;
        char* end = g_render_buffer + sizeof(g_render_buffer);
        for (int i = 0; i < g_assembler_error_count; i++) {
            cur += snprintf(
                cur, end - cur, "<div>line %d: %s</div>", g_assembler_errors[i].line, g_assembler_errors[i].msg);
        }

        populate_selector_with_html("#error-viewer", g_render_buffer);
    }
}

void editor_component(char* initial_content) {
    signal_set(g_editor_text, initial_content);
    populate_selector_with_html("#editor-container", EDITOR_TEMPLATE);
    populate_selector_with_html("#editor", initial_content);

    effect_create(g_runtime, update_line_numbers);
    effect_create(g_runtime, try_assemble);

    EM_ASM({
        document.getElementById('editor').addEventListener('keydown', (e) => {
            if (e.keyCode == 9) {
                e.preventDefault();

                var range = window.getSelection().getRangeAt(0);
                var tabNode = document.createTextNode("\u00a0\u00a0\u00a0\u00a0");

                range.insertNode(tabNode);
                range.setStartAfter(tabNode);
                range.setEndAfter(tabNode);

                return true;
            } else if (e.key == 'Enter') {
                document.execCommand('insertHTML', false, '\n');
                e.preventDefault();
            }
        })
    });
}

EMSCRIPTEN_KEEPALIVE
void editor_on_input(char* input) {
    char* old_editor_text = signal_get(g_editor_text);
    if (old_editor_text)
        free(old_editor_text);

    signal_set(g_editor_text, input);
}

EMSCRIPTEN_KEEPALIVE
void editor_execute() {
    bool started = false;

    pthread_mutex_lock(&g_current_job_mutex);
    rve_load_program(rv_binary, inst_count);
    bool* p_running = signal_get(g_current_job->running);

    if (*p_running == true) {
        *p_running = false;
        g_current_job->stopped = true;
    } else {
        *p_running = true;
        g_current_job->stopped = false;
        started = true;
    }
    pthread_mutex_unlock(&g_current_job_mutex);

    signal_set(g_current_job->running, p_running);
}
