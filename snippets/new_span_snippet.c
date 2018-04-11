#include <stdlib.h>
#include <opentracing-c/tracer.h>

void abc(void)
{
    opentracing_tracer* tracer;
    opentracing_span* span;
    /* ... */
    tracer = opentracing_global_tracer();
    span = tracer->start_span(tracer, "operation_name");
    if (span == NULL) {
        /* Error creating span. */
    }
    /* ... */
    span->finish(span);
    /* ... */
    ((opentracing_destructible*) span)
        ->destroy((opentracing_destructible*) span);
}
