#include <opentracing-c/tracer.h>

#include <assert.h>

static void noop_destroy(opentracing_destructible* destructible)
{
    (void) destructible;
}

#define NOOP_DESTRUCTIBLE_INIT \
    {                          \
        &noop_destroy          \
    }

static void noop_foreach_baggage_item(opentracing_span_context* span_context,
                                      opentracing_bool (*f)(void*,
                                                            const char*,
                                                            const char*),
                                      void* arg)
{
    (void) span_context;
    (void) f;
    (void) arg;
}

static opentracing_span_context noop_span_context_singleton = {
    NOOP_DESTRUCTIBLE_INIT, &noop_foreach_baggage_item};

typedef struct noop_span {
    opentracing_span base;
    opentracing_tracer* tracer_ptr;
} noop_span;

static void
noop_span_finish_with_options(opentracing_span* span,
                              const opentracing_finish_span_options* options)
{
    (void) span;
    (void) options;
    (void) span;
}

static void noop_span_finish(opentracing_span* span)
{
    span->finish_with_options(span, NULL);
}

static opentracing_span_context* noop_span_span_context(opentracing_span* span)
{
    (void) span;
    return &noop_span_context_singleton;
}

static void noop_span_set_operation_name(opentracing_span* span,
                                         const char* operation_name)
{
    (void) span;
    (void) operation_name;
}

static void noop_span_set_tag(opentracing_span* span,
                              const char* key,
                              const opentracing_value* value)
{
    (void) span;
    (void) key;
    (void) value;
}

static void noop_span_log_fields(opentracing_span* span,
                                 const opentracing_log_field* fields,
                                 int num_fields)
{
    (void) span;
    (void) fields;
    (void) num_fields;
}

static void noop_span_set_baggage_item(opentracing_span* span,
                                       const char* key,
                                       const char* value)
{
    (void) span;
    (void) key;
    (void) value;
}

static const char* empty_str = "";

static const char* noop_span_baggage_item(const opentracing_span* span,
                                          const char* key)
{
    (void) span;
    (void) key;
    return empty_str;
}

static opentracing_tracer* noop_span_tracer(const opentracing_span* span)
{
    assert(span != NULL);
    return ((const noop_span*) span)->tracer_ptr;
}

#define NOOP_SPAN_INIT                   \
    {                                    \
        {NOOP_DESTRUCTIBLE_INIT,         \
         &noop_span_finish,              \
         &noop_span_finish_with_options, \
         &noop_span_span_context,        \
         &noop_span_set_operation_name,  \
         &noop_span_set_tag,             \
         &noop_span_log_fields,          \
         &noop_span_set_baggage_item,    \
         &noop_span_baggage_item,        \
         &noop_span_tracer,              \
         NULL,                           \
         0},                             \
            NULL                         \
    }

static noop_span noop_span_singleton = NOOP_SPAN_INIT;

static void noop_tracer_close(opentracing_tracer* tracer)
{
    (void) tracer;
}

static opentracing_span* noop_tracer_start_span_with_options(
    opentracing_tracer* tracer,
    const char* operation_name,
    const opentracing_start_span_options* options)
{
    noop_span* span;
    (void) tracer;
    (void) operation_name;
    (void) options;
    span = &noop_span_singleton;
    if (span->tracer_ptr != tracer) {
        span->tracer_ptr = tracer;
    }
    return (opentracing_span*) span;
}

static opentracing_span* noop_tracer_start_span(opentracing_tracer* tracer,
                                                const char* operation_name)
{
    return tracer->start_span_with_options(tracer, operation_name, NULL);
}

static opentracing_propagation_error_code
noop_tracer_inject(opentracing_tracer* tracer,
                   void* carrier,
                   const opentracing_span_context* span_context)
{
    (void) tracer;
    (void) carrier;
    (void) span_context;
    return opentracing_propagation_error_code_success;
}

static opentracing_propagation_error_code
noop_tracer_extract(opentracing_tracer* tracer,
                    void* carrier,
                    opentracing_span_context** span_context)
{
    (void) tracer;
    (void) carrier;
    assert(span_context != NULL);
    *span_context = &noop_span_context_singleton;
    return opentracing_propagation_error_code_success;
}

#define NOOP_TRACER_INIT                                                     \
    {                                                                        \
        NOOP_DESTRUCTIBLE_INIT, &noop_tracer_close, &noop_tracer_start_span, \
            &noop_tracer_start_span_with_options, &noop_tracer_inject,       \
            &noop_tracer_extract                                             \
    }

static opentracing_tracer noop_tracer_singleton = NOOP_TRACER_INIT;
static opentracing_tracer* global_tracer = &noop_tracer_singleton;

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
