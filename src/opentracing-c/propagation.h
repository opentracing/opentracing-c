#ifndef OPENTRACINGC_PROPAGATION_H
#define OPENTRACINGC_PROPAGATION_H

#include <opentracing-c/config.h>

#include <opentracing-c/common.h>
#include <opentracing-c/span.h>

/** @file */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Error codes for errors occurring in span context propagation. */
typedef enum opentracing_propagation_error_code {
    /** Success value. */
    opentracing_propagation_error_code_success = 0,

    /**
     * Occurs when the format passed to inject() or extract() is not
     * recognized by the tracer implementation.
     */
    opentracing_propagation_error_code_unsupported_format = -1,

    /**
     * Occurs when the carrier passed to extract() is valid and uncorrupted
     * but has insufficient information to extract a span context.
     */
    opentracing_propagation_error_code_span_context_not_found = -2,

    /**
     * Occurs when inject is asked to operate on a span context which it is
     * not prepared to handle (for example, since it was created by a
     * different tracer implementation).
     */
    opentracing_propagation_error_code_invalid_span_context = -3,

    /**
     * Occurs when inject() or extract() implementations expect a different type
     * of carrier than they are given.
     */
    opentracing_propagation_error_code_invalid_carrier = -4,

    /**
     * Occurs when the carrier passed to extract() is of the expected type but
     * is corrupted.
     */
    opentracing_propagation_error_code_span_context_corrupted = -5,

    /**
     * Occurs when a propagation function encounters an unknown exception. This
     * can represent an out-of-memory error, or any other error not covered by
     * the other error codes.
     */
    opentracing_propagation_error_code_unknown = -6
} opentracing_propagation_error_code;

/**
 * Propagation formats.
 */
typedef enum opentracing_propagation_format {
    /** Represents span contexts as opaque binary data. */
    opentracing_propagation_format_binary = 1,

    /**
     * Represents span contexts as key:value string pairs. Unlike
     * opentracing_propagation_format_http_headers, the
     * opentracing_propagation_format_text_map format does not restrict
     * the key or value character sets in any way.
     */
    opentracing_propagation_format_text_map,

    /**
     * Represents span contexts as HTTP header string pairs. Unlike
     * opentracing_propagation_format_text_map, the
     * opentracing_propagation_format_http_headers requires that the
     * keys and values be valid as HTTP headers as-is (i.e., character casing
     * may be unstable and special characters are disallowed in keys, values
     * should be URL-escaped, etc.).
     */
    opentracing_propagation_format_http_headers,

    /**
     * Represents span contexts in a user-defined format. With it, the caller
     * can encode/decode a opentracing_span_context for propagation as entries
     * in a custom protocol.
     */
    opentracing_propagation_format_custom
} opentracing_propagation_format;

/**
 * The inject carrier for the opentracing_propagation_format_text_map.
 * With it, the caller can encode a span context for propagation as entries in a
 * map of strings.
 * @attention The backing store for the opentracing_text_map_writer may
 *            contain data unrelated to span context. As such, inject() and
 *            extract() implementations that call the
 *            opentracing_text_map_writer and opentracing_text_map_reader
 *            interfaces must agree on a prefix or other convention to
 *            distinguish their own key:value pairs.
 * @extends opentracing_destructible
 */
typedef struct opentracing_text_map_writer {
    /** Base class member. */
    opentracing_destructible base;

    /**
     * Set a key:value pair to the carrier. Multiple calls to set() for the
     * same key leads to undefined behavior.
     * @param writer Writer instance.
     * @param key String key.
     * @param value String value.
     * @return opentracing_propagation_error_code indicating success or failure.
     */
    opentracing_propagation_error_code (*set)(
        struct opentracing_text_map_writer* writer,
        const char* key,
        const char* value) OPENTRACINGC_NONNULL(1);
} opentracing_text_map_writer;

/**
 * The extract() carrier for the opentracing_propagation_format_text_map
 * with it, the caller can decode a propagated span context as entries in a map
 * of strings.
 * @attention The backing store for the opentracing_text_map_reader may
 *            contain data unrelated to span context. As such, inject() and
 *            extract() implementations that call the
 *            opentracing_text_map_writer and opentracing_text_map_reader
 *            interfaces must agree on a prefix or other convention to
 *            distinguish their own key:value pairs.
 * @extends opentracing_destructible
 */
typedef struct opentracing_text_map_reader {
    /** Base class member. */
    opentracing_destructible base;

    /**
     * Returns text map contents via repeated calls to the handler function.
     * If any call to handler returns a non-nil error, immediately returns that
     * error. The "foreach" callback pattern reduces unnecessary copying in some
     * cases and also allows implementations to hold locks while the map is
     * read.
     * @param reader Reader instance.
     * @param handler Function to call for each key:value pair. It should accept
     *                a key and a value as arguments and return an error code
     *                indicating success or failure.
     * @return Error code indicating success or failure.
     */
    opentracing_propagation_error_code (*foreach_key)(
        struct opentracing_text_map_reader* reader,
        opentracing_propagation_error_code (*handler)(const char* key,
                                                      const char* value))
        OPENTRACINGC_NONNULL();
} opentracing_text_map_reader;

/**
 * HTTP headers writer.
 * @extends opentracing_text_map_writer
 */
typedef struct opentracing_http_headers_writer {
    /** Base class member. */
    opentracing_text_map_writer writer;
} opentracing_http_headers_writer;

/**
 * HTTP headers reader.
 * @extends opentracing_text_map_reader
 */
typedef struct opentracing_http_headers_reader {
    /** Base class member. */
    opentracing_text_map_reader reader;
} opentracing_http_headers_reader;

/* Forward declaration. */
struct opentracing_tracer;

/**
 * extract() carrier for a custom format. With it, the caller can decode an
 * opentracing_span_context from entries in a custom protocol.
 * @extends opentracing_destructible
 */
typedef struct opentracing_custom_carrier_reader {
    /** Base class member. */
    opentracing_destructible base;

    /**
     * Extract a span context from a custom format.
     * @param reader Reader instance.
     * @param tracer Tracer instance.
     * @param[out] span_context Span context pointer to return decoded span. Set
     *                          to NULL on propagation failure or out of memory.
     * @return Error code indicating success or failure.
     */
    opentracing_propagation_error_code (*extract)(
        struct opentracing_custom_carrier_reader* reader,
        const struct opentracing_tracer* tracer,
        opentracing_span_context** span_context) OPENTRACINGC_NONNULL();
} opentracing_custom_carrier_reader;

/**
 * inject() carrier for a custom format. With it, the caller can encode an
 * opentracing_span_context for propagation as entries in a custom protocol.
 * @extends opentracing_destructible
 */
typedef struct opentracing_custom_carrier_writer {
    /** Base class member. */
    opentracing_destructible base;

    /**
     * Inject a span context into a custom format.
     * @param writer Writer instance.
     * @param tracer Tracer instance.
     * @param span_context Span context to encode.
     * @return Error code indicating success or failure.
     */
    opentracing_propagation_error_code (*inject)(
        struct opentracing_custom_carrier_writer* writer,
        const struct opentracing_tracer* tracer,
        const opentracing_span_context* span_context) OPENTRACINGC_NONNULL();
} opentracing_custom_carrier_writer;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OPENTRACINGC_PROPAGATION_H */
