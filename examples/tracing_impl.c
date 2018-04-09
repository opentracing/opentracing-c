#include <tracing_impl.h>

#include <stdlib.h>

opentracing_tracer* make_tracing_impl()
{
    return malloc(sizeof(tracer_impl));
}
