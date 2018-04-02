#ifndef OPENTRACINGC_TRACER_FACTORY_H
#define OPENTRACINGC_TRACER_FACTORY_H

#include <opentracing-c/config.h>

#include <opentracing-c/common.h>
#include <opentracing-c/tracer.h>

/** @file */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * opentracing_tracer_factory interface to encapsulate vendor tracing libraries.
 * @param config Configuration string to pass to tracer.
 * @param[out] tracer Storage for new tracer instance pointer.
 * @param[out] error_buffer Buffer for potential error message.
 * @param error_buffer_length Length of error_buffer.
 * @return opentracing_true on success, opentracing_false otherwise.
 */
typedef opentracing_bool (*opentracing_tracer_factory)(
    const char* config,
    opentracing_tracer** tracer,
    char* error_buffer,
    int error_buffer_length);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OPENTRACINGC_TRACER_FACTORY_H */
