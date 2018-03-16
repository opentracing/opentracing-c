#ifndef OPENTRACINGC_SPAN_H
#define OPENTRACINGC_SPAN_H

#include <opentracing-c/config.h>
#include <opentracing-c/destructible.h>
#include <opentracing-c/value.h>

/** @file */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Span context interface. Span context represents span state that must be
 * propagated to descendant spans and across boundaries (e.g. trace I:D,
 * span ID, baggage).
 * @extends opentracing_destructible
 */
typedef struct opentracing_span_context {
    /** Base class member. */
    opentracing_destructible base;

    /**
     * Calls a function for each baggage item in the span context. If the
     * function returns opentracing_false, it will not be called again, and
     * foreach_baggage_item will return immediately.
     * @param span_context Span context instance.
     * @param f Callback function. Takes a user-defined argument,
     *          the baggage key, and the baggage value.
     * @param arg Argument to pass to callback function.
     */
    void (*foreach_baggage_item)(struct opentracing_span_context* span_context,
                                 opentracing_bool (*f)(void* arg,
                                                       const char* key,
                                                       const char* value),
                                 void* arg);
} opentracing_span_context;

/**
 * Log field to represent key-value pair for a log.
 */
typedef struct opentracing_log_field {
    /** Key string. Owner of log field must free upon log field destruction. */
    char* key;
    /** Value representation. */
    opentracing_value value;
} opentracing_log_field;

/**
 * Log record can be used to describe events that occur in the lifetime of a
 * span.
 */
typedef struct opentracing_log_record {
    /** Time of logged event. */
    opentracing_timestamp timestamp;
    /** Array of fields. */
    opentracing_log_field* fields;
    /** Number of fields. If fields is NULL, must be set to zero. */
    int num_fields;
} opentracing_log_record;

typedef enum opentracing_span_reference_type {
    /**
     * Refers to a parent span that caused *and* somehow depends
     * upon the new child span. Often (but not always), the parent span cannot
     * finish until the child span does.
     */
    opentracing_span_reference_child_of = 1,

    /**
     * Refers to a parent span that does not depend in any way on the result of
     * the new child span. For instance, one might use "follow from" to describe
     * pipeline stages separated by queues, or a fire-and-forget cache insert at
     * the tail end of a web request.
     *
     * A "follow from" span is part of the same logical trace as the new span:
     * i.e., the new span is somehow caused by the work of its "follow from."
     *
     * All of the following could be valid timing diagrams for children that
     * "follow from" a parent.
     *
     *     [-Parent Span-]  [-Child Span-]
     *
     *
     *     [-Parent Span--]
     *      [-Child Span-]
     *
     *
     *     [-Parent Span-]
     *                 [-Child Span-]
     *
     * See http:*opentracing.io/spec/
     */
    opentracing_span_reference_follows_from = 2
} opentracing_span_reference_type;

typedef struct opentracing_span_reference {
    opentracing_span_reference_type type;
    opentracing_span_context* referenced_context;
} opentracing_span_reference;

#define OPENTRACINGC_CHILD_OF(span_context)                  \
    {                                                        \
        opentracing_span_reference_child_of, &(span_context) \
    }

#define OPENTRACINGC_FOLLOWS_FROM(span_context)                  \
    {                                                            \
        opentracing_span_reference_follows_from, &(span_context) \
    }

typedef struct opentracing_finish_span_options {
    /** Time span finished using monotonic clock. */
    opentracing_duration finish_time;

    /** Array of log records. */
    const opentracing_log_record* log_records;

    /**
     * Number of log records. If log_records is NULL, num_log_records must be
     * set to zero.
     */
    int num_log_records;
} opentracing_finish_span_options;

/* Forward declaration. */
struct opentracing_tracer;

/**
 * Span interface.
 * @extends opentracing_destructible
 */
typedef struct opentracing_span {
    /** Base class member. */
    opentracing_destructible base;

    /**
     * Sets the end timestamp and finalizes span state.
     *
     * With the exception of calls to context (which are always allowed),
     * finish must be the last call made to any span instance, and to do
     * otherwise leads to undefined behavior.
     *
     * @param span Span instance.
     * @see context
     */
    void (*finish)(struct opentracing_span* span) OPENTRACINGC_NONNULL();

    /**
     * Like finish() but with explicit control over timestamps and log data.
     * @param span Span instance.
     * @param options Options to override in span completion.
     * @see finish
     */
    void (*finish_with_options)(struct opentracing_span* span,
                                const opentracing_finish_span_options* options)
        OPENTRACINGC_NONNULL(1);

    /**
     * Yields the opentracing_span_context for this span. Note that the
     * return value of context is still valid after a call to finish, as is
     * a call to context after a call to finish.
     * @param span Span instance.
     * @return Span context associated with this span.
     * @see finish
     */
    opentracing_span_context* (*span_context)(struct opentracing_span* span)
        OPENTRACINGC_NONNULL();

    /**
     * Sets or changes the operation name.
     * @param span Span instance.
     * @param operation_name New operation name.
     */
    void (*set_operation_name)(struct opentracing_span* span,
                               const char* operation_name)
        OPENTRACINGC_NONNULL();

    /**
     * Adds a tag to the span. If there is a pre-existing tag set for `key`,
     * it is overwritten. Tag values can be numeric types, strings, or bools.
     * The behavior of other tag value types is undefined at the OpenTracing
     * level. If a tracing system does not know how to handle a particular
     * value type, it may ignore the tag, but shall not panic.
     * @param span Span instance.
     * @param key Tag key. Value copied into new allocated string.
     * @param value Tag value. Value copied into opentracing_value.
     */
    void (*set_tag)(struct opentracing_span* span,
                    const char* key,
                    const opentracing_value* value) OPENTRACINGC_NONNULL();

    /**
     * Record key:value logging data about a span.
     * @param span Span instance.
     * @param fields Array of log fields. Values must be copied from argument.
     *               Caller must free fields when finished using them.
     * @param num_fields Number of log fields.
     * @see finish_with_options
     */
    void (*log_fields)(struct opentracing_span* span,
                       const opentracing_log_field* fields,
                       int num_fields) OPENTRACINGC_NONNULL(1);

    /**
     * Sets a key:value pair on this span and its span context that also
     * propagates to descendants of this span. set_baggage_item() enables
     * powerful functionality given a full-stack opentracing integration
     * (e.g. arbitrary application data from a mobile app can make it,
     * transparently, all the way into the depths of a storage system), and
     * with it some powerful costs: use this feature with care.
     * @attention set_baggage_item() will only propagate baggage items to
     *            future causal descendants of the associated span.
     * @attention Use this thoughtfully and with care. Every key and
     *            value is copied into every local and remote child of the
     *            associated span, and that can add up to a lot of network and
     *            CPU overhead.
     * @param span Span instance.
     * @param key Baggage key.
     * @param value Baggage value.
     */
    void (*set_baggage_item)(struct opentracing_span* span,
                             const char* key,
                             const char* value) OPENTRACINGC_NONNULL();

    /**
     * Gets the value for a baggage item given its key.
     * @param span Span instance.
     * @param key Baggage key.
     * @return If baggage key found, returns baggage value, otherwise returns
     *         empty string.
     */
    const char* (*baggage_item)(const struct opentracing_span* span,
                                const char* key) OPENTRACINGC_NONNULL();

    /**
     * Provides access to the tracer that created this span.
     * @param span Span instance.
     * @return Tracer instance that created this span.
     */
    struct opentracing_tracer* (*tracer)(const struct opentracing_span* span)
        OPENTRACINGC_NONNULL();

    /**
     * Get magic number used to uniquely identify tracing vendor span type.
     * Necessary to cast span to vendor's type. This number must reflect an
     * ABI-compatible struct, so it must be adjusted any time members are
     * changed in the vendor span struct.
     * @param span Span instance.
     * @return Magic number of the span implementation.
     */
    int (*magic)(const struct opentracing_span* span) OPENTRACINGC_NONNULL();
} opentracing_span;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OPENTRACINGC_SPAN_H */
