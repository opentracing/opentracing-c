#ifndef TRACING_IMPL_H
#define TRACING_IMPL_H

#include <opentracing-c/tracer.h>

typedef struct tracer_impl {
    opentracing_tracer base;
} tracer_impl;

opentracing_tracer* make_tracing_impl();

#endif /* TRACING_IMPL_H */
