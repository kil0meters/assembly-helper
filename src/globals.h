#pragma once

#include "challenge.h"
#include "lesson.h"
#include "signals.h"

Challenge* g_current_challenge = NULL;
Lesson* g_current_lesson = NULL;
Runtime* g_runtime;

typedef struct ErrorMessage {
    uint32_t line;
    char msg[128];
} ErrorMessage;

#define MAX_ASSEMBLER_ERRORS 32
ErrorMessage g_assembler_errors[MAX_ASSEMBLER_ERRORS] = {0};
size_t g_assembler_error_count = 0;
