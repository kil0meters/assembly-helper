#!/bin/bash

emcc \
    --no-entry \
    -std=gnu23 \
    -O3 \
    -flto \
    --closure 1 \
    -pthread \
    -s ASYNCIFY \
    -s PTHREAD_POOL_SIZE=2 \
    -s PTHREAD_POOL_SIZE_STRICT=2 \
    -s FILESYSTEM=0 \
    -s MALLOC=emmalloc \
    -s ENVIRONMENT=web,worker \
    -s EXPORTED_FUNCTIONS='["_init", "_render_on_route", "_toggle_sidebar", "_editor_on_input", "_editor_execute"]' \
    -s DEFAULT_LIBRARY_FUNCS_TO_INCLUDE='["$UTF8ToString", "$stringToNewUTF8"]' \
    -o static/main.js \
    src/main.c \
    src/rve_moss/arr.c src/rve_moss/exec.c src/rve_moss/inst.c src/rve_moss/parse.c src/rve_moss/register.c src/rve_moss/risc-v.c
