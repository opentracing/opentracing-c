#ifndef OPENTRACINGC_TRACER_H
#define OPENTRACINGC_TRACER_H

#include <opentracing-c/config.h>
#include <opentracing-c/propagation.h>
#include <opentracing-c/span.h>
#include <opentracing-c/visibility.h>

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
     * Close the tracer. This may do nothing, flush pending spans, or whatever
     * else a tracing implementation decides close should mean. If not called
     * before the destruction of the tracer, the tracer must perform the
     * equivalent of close upon destruction.
     * @attention The main distinction between close and destroy is whether or
     *            not the tracer is in a valid state after the call.
     *            Upon destruction, subsequent use of a tracer may result in
     *            undefined behavior (i.e. members may be set to NULL causing
     *            deference of NULL member). However, after close, the tracer
     *            must not remain in an undefined state.
     *
     * @param tracer Tracer instance.
     */
    void (*close)(struct opentracing_tracer* tracer) OPENTRACINGC_NONNULL_ALL;

    /**
     * Equivalent to calling start_span_with_options with options as NULL.
     * @param tracer Tracer instance.
     * @param operation_name Name of operation associated with span.
     * @return Span pointer on success, NULL otherwise.
     */
    opentracing_span* (*start_span)(struct opentracing_tracer* tracer,
                                    const char* operation_name)
        OPENTRACINGC_NONNULL_ALL;

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
        const opentracing_start_span_options* options) OPENTRACINGC_NONNULL(1,
                                                                            2);

    /**
     * Inject span context into text map carrier.
     * @param tracer Tracer instance.
     * @param carrier Text map carrier.
     * @param span_context Span context to serialize to carrier.
     * @return Error code indicating success or failure.
     */
    opentracing_propagation_error_code (*inject_text_map)(
        struct opentracing_tracer* tracer,
        opentracing_text_map_writer* carrier,
        const opentracing_span_context* span_context) OPENTRACINGC_NONNULL_ALL;

    /**
     * Inject span context into HTTP headers carrier.
     * @param tracer Tracer instance.
     * @param carrier HTTP headers carrier.
     * @param span_context Span context to serialize to carrier.
     * @return Error code indicating success or failure.
     */
    opentracing_propagation_error_code (*inject_http_headers)(
        struct opentracing_tracer* tracer,
        opentracing_http_headers_writer* carrier,
        const opentracing_span_context* span_context) OPENTRACINGC_NONNULL_ALL;

    /**
     * Inject span context into binary carrier.
     * @param tracer Tracer instance.
     * @param callback Binary carrier callback. Passed a user-defined argument,
     *                 the binary data chunk, and the length of the binary data
     *                 chunk. May be called multiple times in a row to write
     *                 entire span context. Return non-zero to indicate write
     *                 error.
     * @param arg Binary carrier callback argument.
     * @param span_context Span context to serialize to carrier.
     * @return Error code indicating success or failure.
     */
    opentracing_propagation_error_code (*inject_binary)(
        struct opentracing_tracer* tracer,
        int (*callback)(void*, const char*, size_t),
        void* arg,
        const opentracing_span_context* span_context)
        OPENTRACINGC_NONNULL(1, 2, 4);

    /**
     * Inject span context into custom carrier.
     * @param tracer Tracer instance.
     * @param carrier Custom carrier.
     * @param span_context Span context to serialize to carrier.
     * @return Error code indicating success or failure.
     */
    opentracing_propagation_error_code (*inject_custom)(
        struct opentracing_tracer* tracer,
        opentracing_custom_carrier_writer* carrier,
        const opentracing_span_context* span_context) OPENTRACINGC_NONNULL_ALL;

    /**
     * Extract span context from carrier.
     * @note If the tracer cannot allocate a span, it should return the
     *       opentracing_propagation_error_code_unknown error code and set
     *       span_context to NULL.
     * @param tracer Tracer instance.
     * @param carrier Text map carrier.
     * @param[out] span_context Span context pointer to return decoded span.
     *                          Set to NULL on propagation failure or out of
     *                          memory.
     * @return Error code indicating success or failure.
     */
    opentracing_propagation_error_code (*extract_text_map)(
        struct opentracing_tracer* tracer,
        opentracing_text_map_reader* carrier,
        opentracing_span_context** span_context) OPENTRACINGC_NONNULL_ALL;

    /**
     * Extract span context from carrier.
     * @note If the tracer cannot allocate a span, it should return the
     *       opentracing_propagation_error_code_unknown error code and set
     *       span_context to NULL.
     * @param tracer Tracer instance.
     * @param carrier HTTP headers carrier.
     * @param[out] span_context Span context pointer to return decoded span.
     *                          Set to NULL on propagation failure or out of
     *                          memory.
     * @return Error code indicating success or failure.
     */
    opentracing_propagation_error_code (*extract_http_headers)(
        struct opentracing_tracer* tracer,
        opentracing_http_headers_reader* carrier,
        opentracing_span_context** span_context) OPENTRACINGC_NONNULL_ALL;

    /**
     * Extract span context from carrier.
     * @note If the tracer cannot allocate a span, it should return the
     *       opentracing_propagation_error_code_unknown error code and set
     *       span_context to NULL.
     * @param tracer Tracer instance.
     * @param callback Binary carrier callback. Passed user-defined argument,
     *                 data buffer, and data buffer length. May be called
     *                 multiple times to provide data from binary stream. Must
     *                 return number of bytes read, with a negative number
     *                 indicating an error, and zero indicating end of stream.
     * @param arg Binary carrier callback argument.
     * @param[out] span_context Span context pointer to return decoded span.
     *                          Set to NULL on propagation failure or out of
     *                          memory.
     * @return Error code indicating success or failure.
     */
    opentracing_propagation_error_code (*extract_binary)(
        struct opentracing_tracer* tracer,
        int (*callback)(void*, char*, size_t),
        void* arg,
        opentracing_span_context** span_context) OPENTRACINGC_NONNULL(1, 2, 4);

    /**
     * Extract span context from carrier.
     * @note If the tracer cannot allocate a span, it should return the
     *       opentracing_propagation_error_code_unknown error code and set
     *       span_context to NULL.
     * @param tracer Tracer instance.
     * @param carrier Custom carrier.
     * @param[out] span_context Span context pointer to return decoded span.
     *                          Set to NULL on propagation failure or out of
     *                          memory.
     * @return Error code indicating success or failure.
     */
    opentracing_propagation_error_code (*extract_custom)(
        struct opentracing_tracer* tracer,
        opentracing_custom_carrier_reader* carrier,
        opentracing_span_context** span_context) OPENTRACINGC_NONNULL_ALL;
} opentracing_tracer;

/**
 * Get the tracer singleton. At process start, set to a no-op tracer.
 * @return Global tracer instance.
 * @attention Do not modify members.
 * @see opentracing_init_global_tracer()
 */
OPENTRACINGC_EXPORT opentracing_tracer* opentracing_global_tracer(void);

/**
 * Install a global tracer. Ideally, only called once. Good candidate for use
 * of pthread_once.
 * @param tracer New global tracer instance.
 * @see opentracing_global_tracer()
 */
OPENTRACINGC_EXPORT void opentracing_init_global_tracer(
    opentracing_tracer* tracer) OPENTRACINGC_NONNULL_ALL;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OPENTRACINGC_TRACER_H */
