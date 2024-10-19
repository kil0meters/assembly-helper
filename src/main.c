#include <emscripten.h>

#include "components/base.h"
#include "components/lesson_overview.h"
#include "components/lesson_list.h"

EM_JS(void, populate_selector_with_html, (const char *selector, const char *innerHTML), {
    let element = document.querySelector(UTF8ToString(selector));
    element.innerHTML = UTF8ToString(innerHTML);
});

EM_JS(void, jlog, (const char *val), {
    console.log(UTF8ToString(val));
});

// Initialize function
EMSCRIPTEN_KEEPALIVE void init() {
    char buffer[HTML_BUFFER_SIZE] = "wow";
    // lesson_overview_template(buffer, "this is a title");
    // populate_selector_with_html("#lesson-content", buffer);

    LessonListItem items[3] = {
        { .title = "Foo", .slug = "/foo" },
        { .title = "Bar", .slug = "/bar" },
        { .title = "Baz", .slug = "/baz" },
    };

    lesson_list_template(buffer, 3, items);
    jlog(buffer);
    populate_selector_with_html("#content", buffer);

    emscripten_run_script("console.log('Initialized')");
}
