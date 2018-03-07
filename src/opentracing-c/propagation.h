#ifndef OPENTRACINGC_PROPAGATION_H
#define OPENTRACINGC_PROPAGATION_H

#include <opentracing-c/config.h>

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
    opentracing_propagation_error_code_span_context_corrupted = -5
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
} opentracing_propagation_format;

/**
 * @todo Custom format and custom carrier interface.
 */

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
 */
typedef struct opentracing_text_map_writer {
    /**
     * Set a key:value pair to the carrier. Multiple calls to set() for the
     * same key leads to undefined behavior.
     * @param key String key
     * @param value String value
     * @return opentracing_propagation_error_code indicating success or failure.
     */
    opentracing_propagation_error_code (*set)(const char* key,
                                              const char* value);
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
 */
typedef struct opentracing_text_map_reader {
    /**
     * Returns text map contents via repeated calls to the handler function.
     * If any call to handler returns a non-nil error, immediately returns that
     * error. The "foreach" callback pattern reduces unnecessary copying in some
     * cases and also allows implementations to hold locks while the map is
     * read.
     * @param handler Function to call for each key:value pair. It should accept
     *                a key and a value as arguments, return zero on success,
     *                and a non-zero propagation error code on failure.
     * @return Zero on success, error code on failure.
     */
    int (*foreach_key)(int (*handler)(const char* key, const char* value));
} opentracing_text_map_reader;

/**
 * HTTP headers writer.
 * @extends opentracing_http_headers_writer
 */
typedef struct opentracing_http_headers_writer {
    /** Base class. */
    opentracing_text_map_writer writer;
} opentracing_http_headers_writer;

/**
 * HTTP headers reader.
 * @extends opentracing_http_headers_reader
 */
typedef struct opentracing_http_headers_reader {
    /** Base class. */
    opentracing_text_map_reader reader;
} opentracing_http_headers_reader;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OPENTRACINGC_PROPAGATION_H */
