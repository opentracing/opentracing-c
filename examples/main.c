#include "tracing_impl.h"
#include <assert.h>
#include <opentracing-c/tracer.h>

int main(void)
{
    opentracing_tracer* tracer;
    tracer = make_tracing_impl();
    assert(tracer != NULL);
    opentracing_init_global_tracer(tracer);
}
