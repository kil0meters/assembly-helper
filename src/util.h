#pragma once

char ERROR_BUF[1024];

EM_JS(void, jlog, (const char *val), {
    console.log(UTF8ToString(val));
});

EM_JS(char *, local_storage_get, (const char *key), {
    return stringToNewUTF8(localStorage.getItem(UTF8ToString(key)));
});

EM_JS(void, local_storage_set, (const char *key, const char* value), {
    localStorage.setItem(UTF8ToString(key), UTF8ToString(value));
});

EM_JS(void, populate_selector_with_html, (const char *selector, const char *innerHTML), {
    let element = document.querySelector(UTF8ToString(selector));
    element.innerHTML = UTF8ToString(innerHTML);
});

EM_JS(void, set_element_classes, (const char *selector, const char *className), {
    let element = document.querySelector(UTF8ToString(selector));
    element.className = UTF8ToString(className);
});

EM_JS(char *, get_element_classes, (const char *selector), {
    let element = document.querySelector(UTF8ToString(selector));
    return stringToNewUTF8(element.className);
});
