#pragma once

#include <emscripten.h>

#include "../rve_moss/risc-v.h"
#include "../util.h"
#include "base.h"
#include "emulator_registers.h"

const char* CONWAY_TEMPLATE = QUOTE(
    <div id="conway">
        %s
    </div>
);

void render_conway(char* out) {
    char* cur = out;
    char* end = out + HTML_BUFFER_SIZE * 4;

    for (int i = 0; i < 400; i++) {
        if (rve_memory[0x1000 + i] != 0) {
            cur += snprintf(cur, end - cur, "<div class=\"o\"></div>");
        } else {
            cur += snprintf(cur, end - cur, "<div class=\"i\"></div>");
        }
    }
}

EMSCRIPTEN_KEEPALIVE
void step_conway() {
    for (int i = 0; i < 50000; i++) {
        uint32_t inst = rve_get_memory_u32(rve_register_get(RVE_PC_REGISTER));
        rve_parse_and_exec(inst);
    }

    char buffer[HTML_BUFFER_SIZE * 4];
    render_conway(buffer);
    populate_selector_with_html("#conway", buffer);
    emulator_registers_template(buffer);
    populate_selector_with_html("#emulator-registers", buffer);
}
