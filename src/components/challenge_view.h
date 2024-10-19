#pragma once

#include "base.h"
#include "editor.h"

const char *CHALLENGE_VIEW_TEMPLATE = QUOTE(
    <div class="dynamic-page">
        <h1><a href="%s">Assembly Playground</a> / %s</h1>

        <div id="editor-container">
        </div>

        <button id="toggle-fullscreen-button" onclick="Module._toggle_sidebar()">
            <?xml version="1.0" ?><svg height="24px" version="1.1" viewBox="0 0 14 14" width="24px" xmlns="http://www.w3.org/2000/svg" xmlns:sketch="http://www.bohemiancoding.com/sketch/ns" xmlns:xlink="http://www.w3.org/1999/xlink"><title/><desc/><defs/><g fill="none" fill-rule="evenodd" id="Page-1" stroke="none" stroke-width="1"><g fill="#000000" id="Core" transform="translate(-215.000000, -257.000000)"><g id="fullscreen" transform="translate(215.000000, 257.000000)"><path d="M2,9 L0,9 L0,14 L5,14 L5,12 L2,12 L2,9 L2,9 Z M0,5 L2,5 L2,2 L5,2 L5,0 L0,0 L0,5 L0,5 Z M12,12 L9,12 L9,14 L14,14 L14,9 L12,9 L12,12 L12,12 Z M9,0 L9,2 L12,2 L12,5 L14,5 L14,0 L9,0 L9,0 Z" id="Shape"/></g></g></g></svg>
        </button>

        <div class="execution-buddy">
            <div id="error-viewer"></div>

            <span class="p-4 pt-8 pl-12 ">Instruction Count</span> <span id="instruction-count" class="p-4 pr-12 pt-8 cycle-value text-right">---</span>
            <span class="p-4 pl-12">Target</span> <span class="p-4 pr-12 cycle-value text-right">%llu cycles</span>
            <span class="p-4 pl-12 pb-8">Last Execution</span> <span id="last-execution-cycles" class="p-4 pr-12 pb-8 cycle-value text-right">--- cycles</span>

            <button id="execute-button" onclick="Module._editor_execute(stringToNewUTF8(document.getElementById('editor').value))">execute</button>
        </div>
    </div>
);

const char *CHALLENGE_SIDEBAR_TEMPLATE = QUOTE(
    <div class="flex flex-col w-100">
        <div id="completion-status">
        </div>

        <div id="emulator-registers">
            %s
        </div>

        <div class="p-16 description-container">
            %s
        </div>
    </div>
);

void challenge_view_template(char *out, Lesson *lesson, Challenge *challenge) {
    snprintf(out, HTML_BUFFER_SIZE, CHALLENGE_VIEW_TEMPLATE, lesson->slug, challenge->title, challenge->clear_condition.required_cycle_count);
}

void challenge_sidebar_template(char *out, Challenge *challenge) {
    char emulator_registers[HTML_BUFFER_SIZE];
    emulator_registers_template(emulator_registers);

    snprintf(out, HTML_BUFFER_SIZE, CHALLENGE_SIDEBAR_TEMPLATE, emulator_registers, challenge->description);
}
