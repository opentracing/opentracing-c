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

/**
 * Tracer interface.
 * @extends opentracing_destructible
 */
typedef struct opentracing_tracer {
    /** Base class member. */
    opentracing_destructible base;

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
        const char* operation_name,
        const opentracing_start_span_options* options);

    /**
     * Inject span context into carrier.
     * @param tracer Tracer instance.
     * @param format Propagation format.
     * @param carrier Opaque carrier.
     * @param span_context Span context to serialize to carrier.
     * @return Zero on success, error code on propagation failure.
     */
    int (*inject)(struct opentracing_tracer* tracer,
                  opentracing_propagation_format format,
                  void* carrier,
                  const opentracing_span_context* span_context);

    /**
     * Extract span context from carrier.
     * @attention If the tracer cannot allocate a span, it should return zero
     *            and set span_context to NULL. Callers should note that a
     *            return value of zero does not mean the span_context is not
     *            NULL.
     * @param tracer Tracer instance.
     * @param format Propagation format.
     * @param carrier Opaque carrier.
     * @param[out] span_context Span context pointer to return decoded span.
     *                          Set to NULL on propagation failure or out of
     *                          memory.
     * @return Zero on success, error code on propagation failure.
     */
    int (*extract)(struct opentracing_tracer* tracer,
                   opentracing_propagation_format format,
                   void* carrier,
                   opentracing_span_context** span_context);
} opentracing_tracer;

/**
 * Get the tracer singleton. At process start, set to a no-op tracer.
 * @return Global tracer instance.
 * @attention Do not modify members.
 * @see opentracing_init_global_tracer()
 */
opentracing_tracer* opentracing_global_tracer(void);

/**
 * Install a global tracer. Ideally, only called once. Good candidate for use
 * of pthread_once.
 * @param tracer New global tracer instance.
 * @see opentracing_global_tracer()
 */
void opentracing_init_global_tracer(opentracing_tracer* tracer);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OPENTRACINGC_TRACER_H */
