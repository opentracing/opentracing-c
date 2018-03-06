#ifndef OPENTRACINGC_TRACER_H
#define OPENTRACINGC_TRACER_H

#include <opentracing-c/propagation.h>
#include <opentracing-c/span.h>

/** @file */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Simple tag representation. */
typedef struct opentracing_tag {
    /** String key. */
    char* key;
    /** Generic value type. */
    opentracing_value value;
} opentracing_tag;

/** Options used when calling start_span_with_options(). */
typedef struct opentracing_start_span_options {
    /**
     * Start time using monotonic clock.
     */
    opentracing_duration start_time_steady;

    /**
     * Start time using realtime clock.
     */
    opentracing_timestamp start_time_system;

    /**
     * Array of references. May be NULL.
     */
    const opentracing_span_reference* references;

    /**
     * Number of references in array. If references is NULL, num_references
     * must be zero.
     */
    int num_references;

    /**
     * Array of tags. May be NULL.
     */
    const opentracing_tag* tags;

    /**
     * Number of tags in array. If tags is NULL, num_tags must be zero.
     */
    int num_tags;
} opentracing_start_span_options;

/** Tracer interface. */
typedef struct opentracing_tracer {
    /**
     * Equivalent to calling start_span_with_options with options as NULL.
     * @param tracer Tracer instance.
     * @param operation_name Name of operation associated with span.
     * @return Span pointer on success, NULL otherwise.
     */
    opentracing_span* (*start_span)(struct opentracing_tracer* tracer,
                                    const char* operation_name);

    /**
     * Start a new span with provided options.
     * @param tracer Tracer instance.
     * @param operation_name Name of operation associated with span.
     * @param options Options to override default span initialization values.
     * @return Span pointer on success, NULL otherwise.
     */
    opentracing_span* (*start_span_with_options)(
        struct opentracing_tracer* tracer,
        const opentracing_start_span_options* options);

    /**
     * Inject span context into carrier.
     * @param tracer Tracer instance.
     * @param format Propagation format.
     * @param carrier Opaque carrier.
     * @return Zero on success, error code on failure.
     */
    int (*inject)(struct opentracing_tracer* tracer,
                  opentracing_propagation_format format,
                  void* carrier);
} opentracing_tracer;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OPENTRACINGC_TRACER_H */
