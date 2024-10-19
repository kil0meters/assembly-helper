#pragma once
#include "base.h"
#include "emulator_registers.h"
#include "completion_status.h"

#include "../emu/RISCVAssembler.h"

const char *EDITOR_TEMPLATE = QUOTE(
    <div id="line-numbers"></div>
    <textarea spellcheck="false" id="editor"></textarea>
);

void update_line_numbers(char *content) {
    char render_buffer[HTML_BUFFER_SIZE];
    char *cur = render_buffer;
    char *end = render_buffer + sizeof(render_buffer);

    int line_number = 1;
    cur += snprintf(cur, end - cur, "<span>%d</span>", line_number++);
    char *c = content;
    while (*c != '\0') {
        if (*c == '\n') {
            cur += snprintf(cur, end - cur, "<span>%d</span>", line_number++);
        }
        c++;
    }

    populate_selector_with_html("#line-numbers", render_buffer);
}

void render_editor(char *content) {
    populate_selector_with_html("#editor-container", EDITOR_TEMPLATE);

    update_line_numbers(content);
    populate_selector_with_html("#editor", content);

    // sad JS :(
    EM_ASM({
        document.getElementById('editor').addEventListener('keydown', (e) => {
            if(e.keyCode === 9){
                event.preventDefault();
                var range = window.getSelection().getRangeAt(0);
                var tabNode = document.createTextNode("\u00a0\u00a0\u00a0\u00a0");
                range.insertNode(tabNode);

                range.setStartAfter(tabNode);
                range.setEndAfter(tabNode);
            }
        });

        document.getElementById('editor').addEventListener('input', (e) => {
            Module._editor_on_input(stringToNewUTF8(e.target.value))
        });
    });
}

RISCVWord *rv_binary;

EMSCRIPTEN_KEEPALIVE
void editor_on_input(char *input) {
    update_line_numbers(input);
    free(input);
}

EMSCRIPTEN_KEEPALIVE
void editor_execute(char *input) {
    char buffer[HTML_BUFFER_SIZE];
    uint32_t count;
    rv_binary = rva_assemble(&count, (StrA){ .str = input, .length = strlen(input) });

    if (rv_binary != NULL) {
        char *cur = buffer;
        char *end = buffer + sizeof(buffer);
        for (int i = 0; i < count; i++) {
            cur += snprintf(cur, end - cur, " %08x", rv_binary[i]);
        }

        puts(buffer);

        snprintf(buffer, HTML_BUFFER_SIZE, "%u", count);
        populate_selector_with_html("#instruction-count", buffer);
        populate_selector_with_html("#error-viewer", "");

        rve_init();
        rve_load_program(rv_binary, count);
        rve_exec_program();

        emulator_registers_template(buffer);
        populate_selector_with_html("#emulator-registers", buffer);
        populate_selector_with_html("#error-viewer", "");

        bool status = check_clear_condition(current_challenge->clear_condition);
        if (status) {
            current_challenge->complete = true;
            save_challenge_progress();
        }
        completion_status_template(buffer, status);
        populate_selector_with_html("#completion-status", buffer);
    } else {
        char render_buffer[HTML_BUFFER_SIZE];
        snprintf(render_buffer, HTML_BUFFER_SIZE, "<div>%s</div>", ERROR_BUF);

        populate_selector_with_html("#error-viewer", render_buffer);
    }

    free(input);
}
