#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// https://stackoverflow.com/a/2141327
void* realloc_zero(void* pBuffer, size_t oldSize, size_t newSize) {
    void* pNew = realloc(pBuffer, newSize);
    if (newSize > oldSize && pNew) {
        size_t diff = newSize - oldSize;
        void* pStart = ((char*)pNew) + oldSize;
        memset(pStart, 0, diff);
    }
    return pNew;
}

typedef int32_t EffectId;
typedef size_t SignalId;

// TODO: use hashset for performance
typedef struct SignalSubscriberList SignalSubscriberList;
struct SignalSubscriberList {
    EffectId effect_id;
    SignalSubscriberList* next;
};

SignalSubscriberList* ssn_insert_unique(SignalSubscriberList* head, size_t effect_id) {
    SignalSubscriberList* current = head;
    while (current != NULL) {
        if (current->effect_id == effect_id) {
            return head;
        }
        current = current->next;
    }

    SignalSubscriberList* new_node = malloc(sizeof(SignalSubscriberList));
    new_node->effect_id = effect_id;
    new_node->next = head;

    return new_node;
}

SignalSubscriberList* ssn_remove(SignalSubscriberList* head, size_t effect_id) {
    SignalSubscriberList** indirect = &head;

    while (*indirect != NULL) {
        if ((*indirect)->effect_id == effect_id) {
            SignalSubscriberList* node_to_remove = *indirect;
            *indirect = node_to_remove->next;
            free(node_to_remove);
            return head;
        }
        indirect = &(*indirect)->next;
    }

    return head;
}

void ssn_free(SignalSubscriberList* head) {
    while (head != NULL) {
        SignalSubscriberList* next = head->next;
        free(head);
        head = next;
    }
}

typedef struct Runtime {
    size_t signal_capacity;                    // always equal to 1 + the highest valid signal id
    void** signal_values;                      // signal_values[signal_id] => the value associated with that signal
    SignalSubscriberList** signal_subscribers; // signal_subscribers[signal_id] => a linked list of effects for signal

    EffectId running_effect; // -1 == no running effect
    size_t effect_capacity;
    void (**effects)(void);
} Runtime;

typedef struct Effect {
    Runtime* cx;
    EffectId id;
} Effect;

typedef struct Signal {
    Runtime* cx;
    SignalId id;
} Signal;

void runtime_add_signal_subscriber(Runtime* cx, size_t signal_id) {
    if (cx->running_effect != -1) {
        SignalSubscriberList* subscribed_effects = cx->signal_subscribers[signal_id];
        cx->signal_subscribers[signal_id] = ssn_insert_unique(subscribed_effects, cx->running_effect);
    }
}

// we never destroy the runtime
Runtime* runtime_create() {
    Runtime* cx = malloc(sizeof(Runtime));
    cx->signal_capacity = 10;
    cx->signal_values = realloc_zero(NULL, 0, sizeof(void*) * cx->signal_capacity);
    cx->signal_subscribers = realloc_zero(NULL, 0, sizeof(SignalSubscriberList*) * cx->signal_capacity);

    cx->effect_capacity = 10;
    cx->effects = realloc_zero(NULL, 0, sizeof(void (*)(void)) * cx->effect_capacity);
    cx->running_effect = -1;

    return cx;
}

// clears all existing effects
void runtime_reset(Runtime* runtime) {
    for (EffectId i = 0; i < runtime->effect_capacity; i++) {
        runtime->effects[i] = NULL;
    }
    for (SignalId i = 0; i < runtime->signal_capacity; i++) {
        ssn_free(runtime->signal_subscribers[i]);
        runtime->signal_subscribers[i] = NULL;
    }
}

void runtime_run_effect(Runtime* cx, EffectId effect_id) {
    EffectId prev = cx->running_effect;
    cx->running_effect = effect_id;
    cx->effects[effect_id]();
    cx->running_effect = prev;
}

Signal signal_create(Runtime* cx, void* value) {
    SignalId id = 0;
    for (; id < cx->signal_capacity; id++)
        if (cx->signal_values[id] == NULL)
            break;

    // we couldn't find a slot
    if (id == cx->signal_capacity) {
        cx->signal_capacity *= 2;
        realloc_zero(cx->signal_values, sizeof(void*) * id, sizeof(void*) * cx->signal_capacity);
        realloc_zero(cx->signal_subscribers, sizeof(void*) * id, sizeof(void*) * cx->signal_capacity);
    }

    cx->signal_values[id] = value;
    return (Signal){.cx = cx, .id = id};
}
void* signal_get(Signal signal) {
    runtime_add_signal_subscriber(signal.cx, signal.id);
    return signal.cx->signal_values[signal.id];
}

// warning: must be called on main thread
void signal_set(Signal signal, void* value) {
    signal.cx->signal_values[signal.id] = value;

    // rerun subscribers
    SignalSubscriberList* subcribers = signal.cx->signal_subscribers[signal.id];
    signal.cx->signal_subscribers[signal.id] = NULL; // we set to null since rerunning effects resets subscriptions
    while (subcribers) {
        runtime_run_effect(signal.cx, subcribers->effect_id);
        subcribers = subcribers->next;
    }
    ssn_free(subcribers);
}

Effect effect_create(Runtime* cx, void handler(void)) {
    EffectId i = 0;
    for (; i < cx->effect_capacity; i++)
        if (cx->effects[i] == NULL)
            break;

    // we couldn't find a slot
    if (i == cx->effect_capacity) {
        cx->effect_capacity *= 2;
        realloc_zero(cx->effects, sizeof(void (*)(void)) * i, sizeof(void (*)(void)) * cx->effect_capacity);
    }

    cx->effects[i] = handler;
    Effect effect = {.cx = cx, .id = i};
    runtime_run_effect(cx, effect.id);

    return effect;
}
