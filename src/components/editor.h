#pragma once
#include "base.h"

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
    uint32_t count;
    rv_binary = rva_assemble(&count, (StrA){ .str = input, .length = strlen(input) });

    if (rv_binary != NULL) {
        char buffer[1024];
        char *cur = buffer;
        char *end = buffer + sizeof(buffer);
        for (int i = 0; i < count; i++) {
            cur += snprintf(cur, end - cur, " %08x", rv_binary[i]);
        }

        puts(buffer);

        char buffer2[128];
        snprintf(buffer2, 128, "%u", count);
        populate_selector_with_html("#instruction-count", buffer2);
    }


    free(input);
}
