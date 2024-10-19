#!/bin/bash
emcc \
    --no-entry \
    -s EXPORTED_FUNCTIONS='["_init", "_render_on_route", "_toggle_sidebar", "_editor_on_input"]' \
    -s DEFAULT_LIBRARY_FUNCS_TO_INCLUDE='["$UTF8ToString", "$stringToNewUTF8"]' \
    -o static/main.js src/main.c

# -s EXPORTED_RUNTIME_METHODS='["cwrap", "render_on_route"]'
