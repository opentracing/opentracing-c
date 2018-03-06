#include <opentracing-c/tracer.h>

#include <assert.h>
#include <stdlib.h>

static void null_destroy(opentracing_destructible* destructible)
{
    (void) destructible;
}

#define NULL_DESTRUCTIBLE_INIT \
    {                          \
        &null_destroy          \
    }

static void null_foreach_baggage_item(opentracing_span_context* span_context,
                                      opentracing_bool (*f)(void*,
                                                            const char*,
                                                            const char*),
                                      void* arg)
{
    (void) span_context;
    (void) f;
    (void) arg;
}

#define NULL_SPAN_CONTEXT_INIT                             \
    {                                                      \
        NULL_DESTRUCTIBLE_INIT, &null_foreach_baggage_item \
    }

typedef struct null_span {
    opentracing_span base;
    opentracing_span_context null_span_context;
    opentracing_tracer* null_tracer;
} null_span;

static void allocated_destroy(opentracing_destructible* allocated)
{
    free(allocated);
}

static void null_span_destroy(opentracing_destructible* span)
{
    assert(span != NULL);
    opentracing_destructible* span_context =
        (opentracing_destructible*) &((null_span*) span)->null_span_context;
    span_context->destroy(span_context);

    free(span);
}

static void null_span_finish(opentracing_span* span)
{
    (void) span;
}

static void
null_span_finish_with_options(opentracing_span* span,
                              const opentracing_finish_span_options* options)
{
    (void) span;
    (void) options;
    (void) span;
}

static opentracing_span_context* null_span_span_context(opentracing_span* span)
{
    assert(span != NULL);
    return &((null_span*) span)->null_span_context;
}

static opentracing_bool null_span_set_operation_name(opentracing_span* span,
                                                     const char* operation_name)
{
    (void) span;
    (void) operation_name;
    return opentracing_true;
}

static opentracing_bool null_span_set_tag(opentracing_span* span,
                                          const char* key,
                                          const opentracing_value* value)
{
    (void) span;
    (void) key;
    (void) value;
    return opentracing_true;
}

static opentracing_bool null_span_log_fields(
    opentracing_span* span, const opentracing_log_field* fields, int num_fields)
{
    (void) span;
    (void) fields;
    (void) num_fields;
    return opentracing_true;
}

static opentracing_bool null_span_set_baggage_item(opentracing_span* span,
                                                   const char* key,
                                                   const char* value)
{
    (void) span;
    (void) key;
    (void) value;
    return opentracing_true;
}

static const char* null_span_baggage_item(const opentracing_span* span,
                                          const char* key)
{
    (void) span;
    (void) key;
    static const char* empty_str = "";
    return empty_str;
}

static opentracing_tracer* null_span_tracer(opentracing_span* span)
{
    assert(span != NULL);
    return ((null_span*) span)->null_tracer;
}

#define NULL_SPAN_INIT                   \
    {                                    \
        {{&null_span_destroy},           \
         &null_span_finish,              \
         &null_span_finish_with_options, \
         &null_span_span_context,        \
         &null_span_set_operation_name,  \
         &null_span_set_tag,             \
         &null_span_log_fields,          \
         &null_span_set_baggage_item,    \
         &null_span_baggage_item,        \
         &null_span_tracer},             \
            NULL_SPAN_CONTEXT_INIT, NULL \
    }

static opentracing_span* null_tracer_start_span_with_options(
    opentracing_tracer* tracer,
    const char* operation_name,
    const opentracing_start_span_options* options)
{
    (void) tracer;
    (void) operation_name;
    (void) options;
    null_span* span = (null_span*) malloc(sizeof(null_span));
    if (span == NULL) {
        return NULL;
    }
    *span = (null_span) NULL_SPAN_INIT;
    span->null_tracer = tracer;
    return (opentracing_span*) span;
}

static opentracing_span* null_tracer_start_span(opentracing_tracer* tracer,
                                                const char* operation_name)
{
    return tracer->start_span_with_options(tracer, operation_name, NULL);
}

static int null_tracer_inject(opentracing_tracer* tracer,
                              opentracing_propagation_format format,
                              void* carrier,
                              const opentracing_span_context* span_context)
{
    (void) tracer;
    (void) format;
    (void) carrier;
    (void) span_context;
    return 0;
}

static int null_tracer_extract(opentracing_tracer* tracer,
                               opentracing_propagation_format format,
                               void* carrier,
                               opentracing_span_context** span_context)
{
    (void) tracer;
    (void) format;
    (void) carrier;
    assert(span_context != NULL);
    *span_context =
        (opentracing_span_context*) malloc(sizeof(opentracing_span_context));
    if (*span_context == NULL) {
        return 0;
    }
    **span_context = (opentracing_span_context) NULL_SPAN_CONTEXT_INIT;
    ((opentracing_destructible*) (*span_context))->destroy = &allocated_destroy;
    return 0;
}

#define NULL_TRACER_INIT                                               \
    {                                                                  \
        NULL_DESTRUCTIBLE_INIT, &null_tracer_start_span,               \
            &null_tracer_start_span_with_options, &null_tracer_inject, \
            &null_tracer_extract                                       \
    }

static opentracing_tracer null_tracer_singleton = NULL_TRACER_INIT;
static opentracing_tracer* global_tracer = &null_tracer_singleton;

opentracing_tracer* opentracing_global_tracer(void)
{
    return global_tracer;
}

void opentracing_init_global_tracer(opentracing_tracer* tracer)
{
    assert(tracer != NULL);
    assert(global_tracer != NULL);
    if (global_tracer == tracer) {
        return;
    }
    ((opentracing_destructible*) global_tracer)
        ->destroy((opentracing_destructible*) global_tracer);
    global_tracer = tracer;
}
