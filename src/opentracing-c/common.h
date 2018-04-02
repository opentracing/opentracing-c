#ifndef OPENTRACINGC_COMMON_H
#define OPENTRACINGC_COMMON_H

#include <opentracing-c/config.h>

#include <time.h>

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif /* HAVE_SYS_TIME_H */

/** @file */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Boolean type. */
typedef enum { opentracing_true = 1, opentracing_false = 0 } opentracing_bool;

#ifdef OPENTRACINGC_USE_TIMESPEC

typedef struct timespec opentracing_time_value;

#else

typedef struct opentracing_time_value {
    time_t tv_sec;
    long int tv_nsec;
} opentracing_time_value;

#endif /* OPENTRACINGC_USE_TIMESPEC */

/**
 * Duration type to calculate precise intervals (should use monotonic clock).
 */
typedef struct opentracing_duration {
    /** Duration value. */
    opentracing_time_value value;
} opentracing_duration;

/** Timestamp type to represent absolute time (should use system clock). */
typedef struct opentracing_timestamp {
    /** Timestamp value. */
    opentracing_time_value value;
} opentracing_timestamp;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OPENTRACINGC_COMMON_H */
