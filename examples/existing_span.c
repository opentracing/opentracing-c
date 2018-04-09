#include "existing_span.h"
#include <string.h>

// tag::existing-span-snippet[]

void xyz(opentracing_span* parent_span)
{
    opentracing_tracer* tracer;
    opentracing_span* span;
    opentracing_start_span_options options = {0};
    opentracing_span_reference refs[1];
    opentracing_span_context* parent_context;

    parent_context = parent_span->span_context(parent_span);
    /* ... */
    tracer = opentracing_global_tracer();
    memset(&options, 0, sizeof(options));
    refs[0] =
        (opentracing_span_reference) OPENTRACINGC_CHILD_OF(*parent_context);
    options.references = refs;
    options.num_references = 1;
    span = tracer->start_span_with_options(tracer, "operation_name", &options);
    if (span == NULL) {
        /* Error creating span. */
    }
    /* Finishing the span operation. */
    span->finish(span);
    /* Freeing the span. */
    ((opentracing_destructible*) span)
        ->destroy((opentracing_destructible*) span);
}

// end::existing-span-snippet[]
