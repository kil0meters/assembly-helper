#!/bin/bash
emcc \
    --no-entry \
    -g \
    -s EXPORTED_FUNCTIONS='["_init", "_render_on_route", "_toggle_sidebar", "_editor_on_input", "_editor_execute"]' \
    -s DEFAULT_LIBRARY_FUNCS_TO_INCLUDE='["$UTF8ToString", "$stringToNewUTF8"]' \
    -o static/main.js \
    src/main.c \
    src/rve_moss/arr.c src/rve_moss/exec.c src/rve_moss/inst.c src/rve_moss/parse.c src/rve_moss/register.c src/rve_moss/risc-v.c

# -s EXPORTED_RUNTIME_METHODS='["cwrap", "render_on_route"]'
