#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <opentracing-c/dynamic_load.h>

opentracing_bool mock_tracer_factory(const char* config,
                                     opentracing_tracer** tracer,
                                     char* error_buffer,
                                     int error_buffer_length)
{
    (void) config;
    (void) error_buffer;
    (void) error_buffer_length;
    assert(tracer != NULL);
    *tracer = opentracing_global_tracer();
    return opentracing_true;
}

#ifdef BAD_VERSION
#define OPENTRACINGC_VERSION_COMPAT "0.0.0"
#else
#define OPENTRACINGC_VERSION_COMPAT OPENTRACINGC_VERSION_STRING
#endif /* OPENTRACINGC_BAD_VERSION */

#ifdef DEFINE_HOOK

opentracing_tracer_factory opentracing_make_tracer_factory(
    const char* opentracing_version,
    opentracing_dynamic_load_error_code* return_code,
    char* error_buffer,
    int error_buffer_length)
{
    assert(return_code != NULL);
    if (strcmp(opentracing_version, OPENTRACINGC_VERSION_COMPAT) != 0) {
        snprintf(error_buffer,
                 error_buffer_length,
                 "Tracer expected opentracing-c version %s, called with %s",
                 OPENTRACINGC_VERSION_STRING,
                 opentracing_version);
        *return_code =
            opentracing_dynamic_load_error_code_incompatible_library_versions;
        return NULL;
    }

    *return_code = opentracing_dynamic_load_error_code_success;
    return &mock_tracer_factory;
}

#endif /* DEFINE_HOOK */
