#!/bin/bash
emcc --no-entry -s EXPORTED_FUNCTIONS='["_init"]' -s EXPORTED_RUNTIME_METHODS='["cwrap"]' -o static/main.js src/main.c
