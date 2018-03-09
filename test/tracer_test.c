#include <assert.h>
#include <string.h>

#include <opentracing-c/tracer.h>

static void null_destroy(opentracing_destructible* destructible)
{
    (void) destructible;
}

int main(void)
{
    opentracing_tracer* tracer;
    opentracing_span* span;
    opentracing_span_context* span_context;
    int return_code;
    opentracing_value value;
    opentracing_tracer* global_tracer;
    opentracing_tracer dummy_tracer;

    tracer = opentracing_global_tracer();
    span = tracer->start_span(tracer, "a");
    assert(span != NULL);
    assert(span->span_context(span) != NULL);
    assert(span->tracer(span) == tracer);
    assert(strlen(span->baggage_item(span, "key")) == 0);
    span->set_baggage_item(span, "key", "value");
    span->set_tag(span, "tag", &value);
    span->set_operation_name(span, "operation");
    span->log_fields(span, NULL, 0);

    span->finish(span);
    ((opentracing_destructible*) span)
        ->destroy((opentracing_destructible*) span);

    return_code = tracer->extract(
        tracer, opentracing_propagation_format_binary, NULL, &span_context);
    assert(return_code == 0);
    assert(span_context != NULL);
    return_code = tracer->inject(
        tracer, opentracing_propagation_format_text_map, NULL, span_context);
    ((opentracing_destructible*) span_context)
        ->destroy((opentracing_destructible*) span_context);

    opentracing_init_global_tracer(opentracing_global_tracer());

    global_tracer = opentracing_global_tracer();
    memset(&dummy_tracer, 0, sizeof(dummy_tracer));
    dummy_tracer.base.destroy = &null_destroy;
    opentracing_init_global_tracer(&dummy_tracer);
    assert(opentracing_global_tracer() == &dummy_tracer);
    opentracing_init_global_tracer(global_tracer);
    assert(opentracing_global_tracer() == global_tracer);

    return 0;
}
