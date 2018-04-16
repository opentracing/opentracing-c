#include "text_map_iterator.h"
#include <assert.h>
#include <stdio.h>

typedef struct text_map_reader {
    /* Base class instance. */
    opentracing_text_map_reader base;

    /* Pointer to existing map object. */
    const text_map* map;
} text_map_reader;

static opentracing_propagation_error_code text_map_reader_foreach_key(
    opentracing_text_map_reader* reader,
    opentracing_propagation_error_code (*f)(void*, const char*, const char*),
    void* arg)
{
    text_map_reader* r;
    text_map_iterator* it;
    const char* key;
    const char* value;
    opentracing_propagation_error_code return_code;

    assert(reader != NULL);
    assert(f != NULL);

    r = (text_map_reader*) reader;
    it = text_map_get_iterator(r->map);
    if (it == NULL) {
        /* Failed to allocate iterator. */
        return opentracing_propagation_error_code_unknown;
    }

    for (; text_map_iterator_has_next(it);
         text_map_iterator_next(it, &key, &value)) {
        assert(key != NULL);
        assert(value != NULL);
        return_code = f(arg, key, value);
        if (return_code != opentracing_propagation_error_code_success) {
            goto cleanup;
        }
    }

    return_code = opentracing_propagation_error_code_success;

cleanup:
    text_map_iterator_destroy(it);
    return return_code;
}

static void noop_destroy(opentracing_destructible* d)
{
}

/* Initialize new reader with existing map. */
static void text_map_reader_init(text_map_reader* reader, const text_map* map)
{
    assert(reader != NULL);
    ((opentracing_text_map_reader*) &reader)->foreach_key =
        &text_map_reader_foreach_key;
    ((opentracing_destructible*) &reader)->destroy = &noop_destroy;
    reader->map = map;
}

void extract(const text_map* map)
{
    text_map_reader reader;
    opentracing_tracer* tracer;
    opentracing_span_context* span_context;
    opentracing_propagation_error_code return_code;

    text_map_reader_init(&reader, map);
    tracer = opentracing_global_tracer();
    span_context = NULL;
    return_code = tracer->extract_text_map(
        tracer, (opentracing_text_map_reader*) &reader, &span_context);
    if (return_code != opentracing_propagation_error_code_success) {
        fprintf(stderr,
                "Failed to extract span context, error code = %d\n",
                return_code);
        return;
    }
}
