#pragma once

#include <emscripten.h>
#include <stdint.h>

#include "../rve_moss/risc-v.h"
#include "../util.h"
#include "base.h"

const char* SCREEN_TEMPLATE = QUOTE(
    <canvas id="emulator-screen-canvas"></canvas>
);

void display_component(char* target) {
    uint8_t* out_buffer = malloc(20 * 20 * 4); // rgba buffer

    for (int i = 0; i < 400; i++) {
        uint8_t val = rve_memory[0x1000 + i] * 255;

        out_buffer[4 * i] = val;
        out_buffer[4 * i + 1] = val;
        out_buffer[4 * i + 2] = val;
        out_buffer[4 * i + 3] = 255;
    }

    populate_selector_with_html(target, SCREEN_TEMPLATE);
    write_buffer_to_canvas("#emulator-screen-canvas", out_buffer, 20, 20);

    free(out_buffer);
}
