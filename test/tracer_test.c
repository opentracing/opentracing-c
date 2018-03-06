#include <assert.h>
#include <opentracing-c/tracer.h>

int main()
{
    opentracing_tracer* tracer = opentracing_global_tracer();
    opentracing_span* span = tracer->start_span(tracer, "a");
    assert(span != NULL);
    assert(span->span_context(span) != NULL);
    assert(span->tracer(span) == tracer);
    span->finish(span);
    ((opentracing_destructible*) span)
        ->destroy((opentracing_destructible*) span);
    return 0;
}
