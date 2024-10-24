#pragma once
#include "../util.h"
#include "base.h"

#include "../rve_moss/register.h"

const char* EMULATOR_REGISTERS_TEMPLATE = QUOTE(
    <div class="register-list">
        <span class="register-label">pc</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x0</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x1 (ra)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x2 (sp)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x3 (gp)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x4 (tp)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x5 (t0)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x6 (t1)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x7 (t2)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x8 (s0)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x9 (s1)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x10 (a0)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x11 (a1)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x12 (a2)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x13 (a3)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x14 (a4)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x15 (a5)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x16 (a6)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x17 (a7)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x18 (s2)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x19 (s3)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x20 (s4)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x21 (s5)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x22 (s6)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x23 (s7)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x24 (s8)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x25 (s9)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x26 (s10)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x27 (s11)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x28 (t3)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x29 (t4)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x30 (t5)</span> <span class="register-value">0x%08x</span>
        <span class="register-label">x31 (t6)</span> <span class="register-value">0x%08x</span>
    </div>
);

void emulator_registers_component(char* target) {
    snprintf(g_render_buffer,
             HTML_BUFFER_SIZE,
             EMULATOR_REGISTERS_TEMPLATE,
             rve_register_get(RVE_PC_REGISTER),
             rve_register_get(0),
             rve_register_get(1),
             rve_register_get(2),
             rve_register_get(3),
             rve_register_get(4),
             rve_register_get(5),
             rve_register_get(6),
             rve_register_get(7),
             rve_register_get(8),
             rve_register_get(9),
             rve_register_get(10),
             rve_register_get(11),
             rve_register_get(12),
             rve_register_get(13),
             rve_register_get(14),
             rve_register_get(15),
             rve_register_get(16),
             rve_register_get(17),
             rve_register_get(18),
             rve_register_get(19),
             rve_register_get(20),
             rve_register_get(21),
             rve_register_get(22),
             rve_register_get(23),
             rve_register_get(24),
             rve_register_get(25),
             rve_register_get(26),
             rve_register_get(27),
             rve_register_get(28),
             rve_register_get(29),
             rve_register_get(30),
             rve_register_get(31));

    populate_selector_with_html(target, g_render_buffer);
}
