#ifndef OPENTRACINGC_VALUE_H
#define OPENTRACINGC_VALUE_H

#include <stdint.h>

#include <opentracing-c/common.h>

/** @file */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Value types.
 */
typedef enum opentracing_value_type {
    opentracing_value_bool,
    opentracing_value_double,
    opentracing_value_int64,
    opentracing_value_uint64,
    opentracing_value_string,
    opentracing_value_null
} opentracing_value_type;

/**
 * Tagged union that can represent a number of value types.
 */
typedef struct opentracing_value {
    /** Value type. */
    opentracing_value_type type;
    /** Value storage. */
    union {
        /** Storage for Boolean value. */
        opentracing_bool bool_value;
        /** Storage for double value. */
        double double_value;
        /** Storage for 64 bit signed integer value. */
        int64_t int64_value;
        /** Storage for 64 bit unsigned integer value. */
        uint64_t uint64_value;
        /** Storage for string value. */
        char* string_value;
    } value;
} opentracing_value;

/** @todo Constructor and destructor for generic value. */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OPENTRACINGC_VALUE_H */
