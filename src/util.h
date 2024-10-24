#pragma once
#include <emscripten.h>
#include <stdint.h>

EM_JS(void, write_buffer_to_canvas, (const char *target, uint8_t *buffer_ptr, uint32_t width, uint32_t height), {
    const canvas = document.querySelector(UTF8ToString(target));
    const ctx = canvas.getContext('2d');
    const buffer = HEAPU8.subarray(buffer_ptr, buffer_ptr + 4 * width * height);

    canvas.width = width;
    canvas.height = height;

    const imageData = new ImageData(new Uint8ClampedArray(buffer), width, height);
    ctx.putImageData(imageData, 0, 0);
})

EM_JS(void, jlog, (const char *val), { console.log(UTF8ToString(val));
})

EM_JS(char *, local_storage_get, (const char *key), {
    let val = window.localStorage.getItem(UTF8ToString(key));
    if (val) return stringToNewUTF8(val);
    return null;
})

EM_JS(void, local_storage_set, (const char *key, const char* value), { window.localStorage.setItem(UTF8ToString(key), UTF8ToString(value));
})

EM_JS(void, populate_selector_with_html, (const char *selector, const char *innerHTML), {
    let element = document.querySelector(UTF8ToString(selector));
    element.innerHTML = UTF8ToString(innerHTML);
})

EM_JS(void, set_element_classes, (const char *selector, const char *className), {
    let element = document.querySelector(UTF8ToString(selector));
    element.className = UTF8ToString(className);
})

EM_JS(char *, get_element_classes, (const char *selector), {
    let element = document.querySelector(UTF8ToString(selector));
    return stringToNewUTF8(element.className);
})
