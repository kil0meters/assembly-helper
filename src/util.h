#pragma once

EM_JS(void, jlog, (const char *val), {
    console.log(UTF8ToString(val));
});

EM_JS(char *, local_storage_get, (const char *key), {
    return stringToNewUTF8(localStorage.getItem(UTF8ToString(key)));
});

EM_JS(void, local_storage_set, (const char *key, const char* value), {
    localStorage.setItem(UTF8ToString(key), UTF8ToString(value));
});
