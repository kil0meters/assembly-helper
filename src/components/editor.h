#pragma once
#include "base.h"

const char *EDITOR_TEMPLATE = QUOTE(
    <div id="line-numbers"></div>
    <div contenteditable spellcheck="false" id="editor"></div>
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
            let patt2=new RegExp("<div>","g");
            let patt3=new RegExp("</div>","g");
            let patt4=new RegExp("<br>","g");
            let s=e.target.innerHTML.replace(patt2,"\n").replace(patt3,"").replace(patt4,"");
            Module._editor_on_input(stringToNewUTF8(s))
        });
    });
}

EMSCRIPTEN_KEEPALIVE
void editor_on_input(char *input) {
    update_line_numbers(input);
    free(input);
}
