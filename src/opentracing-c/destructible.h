#ifndef OPENTRACINGC_DESTRUCTIBLE_H
#define OPENTRACINGC_DESTRUCTIBLE_H

#include <opentracing-c/config.h>

/** @file */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Destructible interface. Essentially the same as a virtual destructor in C++,
 * but for C.
 */
typedef struct opentracing_destructible {
    /**
     * Destructor to clean up any resources allocated to the instance.
     * @param destructible Destructible instance.
     */
    void (*destroy)(struct opentracing_destructible* destructible)
        OPENTRACINGC_NONNULL_ALL;
} opentracing_destructible;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OPENTRACINGC_DESTRUCTIBLE_H */
