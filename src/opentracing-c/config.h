#ifndef OPENTRACINGC_CONFIG_H
#define OPENTRACINGC_CONFIG_H

#include <opentracing-c/visibility.h>

/** @file */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define OPENTRACINGC_MAJOR_VERSION 0
#define OPENTRACINGC_MINOR_VERSION 0
#define OPENTRACINGC_PATCH_VERSION 1
#define OPENTRACINGC_VERSION_STRING "0.0.1"

#define HAVE_SYS_TIME_H
/* #undef OPENTRACINGC_USE_TIMESPEC */

#define OPENTRACINGC_HAVE_WEAK_SYMBOLS
#define OPENTRACINGC_HAVE_NONNULL_ATTRIBUTE
#define OPENTRACINGC_HAVE_USED_ATTRIBUTE

#ifdef OPENTRACINGC_HAVE_WEAK_SYMBOLS
#define OPENTRACINGC_WEAK __attribute__((weak))
#endif /* OPENTRACINGC_HAVE_WEAK_SYMBOLS */

/* Ignoring warnings because nonnull is just to help us maintain quality code.
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvariadic-macros"

#ifdef OPENTRACINGC_HAVE_NONNULL_ATTRIBUTE

#define OPENTRACINGC_NONNULL(...) \
    __attribute__((nonnull(__VA_ARGS__)))

#define OPENTRACINGC_NONNULL_ALL __attribute__((nonnull))

#else

#define OPENTRACINGC_NONNULL(...)
#define OPENTRACINGC_NONNULL_ALL

#endif /* OPENTRACINGC_HAVE_NONNULL_ATTRIBUTE */

#pragma GCC diagnostic pop

#ifdef OPENTRACINGC_HAVE_USED_ATTRIBUTE

#define OPENTRACINGC_USED \
    __attribute__((used))

#else

#define OPENTRACINGC_USED

#endif /* OPENTRACINGC_USED */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OPENTRACINGC_CONFIG_H */
