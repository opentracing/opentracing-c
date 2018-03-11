#ifndef OPENTRACINGC_DYNAMIC_LOAD_H
#define OPENTRACINGC_DYNAMIC_LOAD_H

#include <opentracing-c/config.h>
#include <opentracing-c/tracer_factory.h>

/** @file */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Error codes for dynamic loading. */
typedef enum opentracing_dynamic_load_error_code {
    /** Success value. */
    opentracing_dynamic_load_error_code_success = 0,

    /**
     * Occurs when dynamically loading a tracer library fails. Possible reasons
     * could be the library doesn't exist or it is missing the required symbols.
     */
    opentracing_dynamic_load_error_code_failure = -1,

    /**
     * Means dynamic loading of tracing libraries is not supported for the
     * platform used.
     */
    opentracing_dynamic_load_error_code_not_supported = -2,

    /**
     * Occurs if the tracing dynamically loaded library uses an incompatible
     * version of opentracing.
     */
    opentracing_dynamic_load_error_code_incompatible_library_versions = -3
} opentracing_dynamic_load_error_code;

#ifdef OPENTRACINGC_HAVE_WEAK_SYMBOLS

/**
 * Provides a common hook that can be used to create a tracing factory from a
 * dynamically loaded library. Users should prefer to use the function
 * dynamically_load_tracing_library() over calling this function directly.
 * @param opentracing_version Version of opentracing-c used by caller.
 * @param[out] return_code Indicating success or failure.
 * @param[out] error_buffer Buffer for potential error message.
 * @param error_buffer_length Length of error_buffer.
 * @return Pointer to library's tracer factory function, NULL on failure.
 * @see opentracing_dynamically_load_tracing_library()
 */
opentracing_tracer_factory opentracing_make_tracer_factory(
    const char* opentracing_version,
    opentracing_dynamic_load_error_code* return_code,
    char* error_buffer,
    int error_buffer_length) OPENTRACINGC_WEAK;

#endif /* OPENTRACINGC_HAVE_WEAK_SYMBOLS */

/**
 * Handle for maintaining ownership of dynamically loaded shared
 * library.
 */
typedef struct opentracing_library_handle {
    /** Tracer factory function. */
    opentracing_tracer_factory factory;
    /** Raw library handle. */
    void* lib_handle;
} opentracing_library_handle;

/**
 * Destroy handle to stop using this library. To avoid undefined behavior, the
 * caller must avoid doing this until all tracers loaded from this library are
 * also destroyed.
 * @param handle Library handle to destroy.
 */
void opentracing_library_handle_destroy(opentracing_library_handle* handle)
    OPENTRACINGC_NONNULL();

/**
 * Dynamically loads a tracing library and returns a handle that can be used
 * to create tracers.
 * @param lib Shared library name.
 * @param[out] handle Library handle that should be closed once symbol is no
 *                    longer needed.
 * @param[out] error_buffer Buffer for potential error message.
 * @param error_buffer_length Length of error_buffer. If error_buffer is NULL,
 *                            must be zero.
 * @return opentracing_dynamic_load_error_code indicating success or failure.
 * @see opentracing_library_handle_destroy
 */
opentracing_dynamic_load_error_code
opentracing_dynamically_load_tracing_library(const char* lib,
                                             opentracing_library_handle* handle,
                                             char* error_buffer,
                                             int error_buffer_length)
    OPENTRACINGC_NONNULL(1, 2);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OPENTRACINGC_DYNAMIC_LOAD_H */
