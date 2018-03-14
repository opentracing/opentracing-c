# opentracing-c

[![Build Status](https://travis-ci.org/opentracing/opentracing-c.svg?branch=master)](https://travis-ci.org/opentracing/opentracing-c)
[![codecov](https://codecov.io/gh/opentracing/opentracing-c/branch/master/graph/badge.svg)](https://codecov.io/gh/opentracing/opentracing-c)

ANSI C implementation of the OpenTracing API http://opentracing.io.

## Required Reading

In order to understand the C platform API, one must first be familiar with the
[OpenTracing project](http://opentracing.io) and
[terminology](http://opentracing.io/documentation/pages/spec) more generally.

## Doxygen

API docs generated using Doxygen are hosted
[here](opentracing.github.io/opentracing-c) on GitHub pages.

## Compile and install

```
mkdir build
cd build
cmake ..
make
sudo make install
```

To test:

```
make test
```

## API overview for those adding instrumentation

Everyday consumers of this `opentracing-c` package really only need to worry
about a couple of key abstractions: the `opentracing_tracer` `start_span`
function, the `opentracing_span` interface, and binding an `opentracing_tracer`
at `main()`-time. Here are code snippets demonstrating some important use cases.

#### Singleton initialization

The simplest starting point is `opentracing-c/tracer.h`. As early as possible,
call

```c
    #include <opentracing-c/tracer.h>
    #include <some_tracing_impl.h>

    int main()
    {
        opentracing_tracer* tracer;
        tracer = make_some_tracing_impl();
        assert(tracer != NULL);
        opentracing_init_global_tracer(tracer);
        ...
    }
```

##### Non-singleton initialization

If you prefer direct control to singletons, manage ownership of the
`opentracing_tracer` implementation explicitly.

#### Starting an empty trace by creating a "root span"

It's always possible to create a "root" `opentracing_span` with no parent or
other causal reference.

```c
    void xyz(void)
    {
        opentracing_tracer* tracer;
        opentracing_span* span;
        ...
        tracer = /* Some Tracer */
        span = tracer->start_span(tracer, "operation_name");
        if (span == NULL) {
            /* Error creating span. */
            ...
        }
        ...
        span->finish(span);
        ...
    }
```

#### Creating a (child) span given an existing (parent) span

```c
    void xyz(const opentracing_span* parent_span, ...)
    {
        opentracing_tracer* tracer;
        opentracing_span* span;
        opentracing_span_start_options options;
        ...
        tracer = (opentracing_tracer*) /* Some Tracer */
        memset(options, 0, sizeof(options));
        options.refs = parent_span;
        options.num_refs = 1;
        span = tracer->start_span_with_options(
            tracer, "operation_name", &options);
        if (span == NULL) {
            /* Error creating span. */
            ...
        }
        ...
        /* Finishing the span operation. */
        span->finish(span);
        ...
        /* Freeing the span. */
        span->destroy((opentracing_destructible*) span);
        ...
    }
```

#### Inject span context into an opentracing_text_map_writer

```c
    /* Custom map implementation. */
    typedef struct text_map text_map;
    /* Returns opentracing_true if map initialization succeeded, false
       otherwise. */
    /* Returns opentracing_true if map has key, opentracing_false otherwise. */
    opentracing_bool text_map_has_key(const text_map* map, const char* key);
    /* Returns opentracing_true if insertion succeeded, opentracing_false
       otherwise. */
    opentracing_bool text_map_insert(
        text_map* map, const char* key, const char* value);

    typedef struct custom_carrier_writer {
        /* OPENTRACINGC_TEXT_MAP_WRITER_SUBCLASS expands to the following
           function pointer. Not using macro here for clarity. */
        /* Returns opentracing_true on success, opentracing_false otherwise. */
        opentracing_bool (*set)(opentracing_text_map_writer* writer,
                   const char* key,
                   const char* value);

        text_map* map;
    } custom_carrier_writer;

    opentracing_bool custom_carrier_writer_set(
        opentracing_text_map_writer* writer,
        const char* key,
        const char* value)
    {
        opentracing_bool success;
        custom_carrier_writer* w;

        assert(writer != NULL);
        assert(key != NULL);
        assert(value != NULL);

        w = (custom_carrier_writer*) writer;
        if (text_map_has_key(w->map, key) ||
            !text_map_insert(w->map, key, value)) {
            return opentracing_false;
        }
        return opentracing_true;
    }

    opentracing_bool custom_carrier_writer_init(custom_carrier_writer* writer)
    {
        assert(writer != NULL);
        writer->set = &custom_carrier_writer_set;
        return text_map_init(writer->map);
    }

    ...

    custom_carrier_writer writer;
    int return_code;
    if (!custom_carrier_writer_init(&writer)) {
        return;
    }
    return_code = tracer->inject(tracer,
                                 opentracing_propagation_format_binary,
                                 (opentracing_text_map_writer*) carrier,
                                 &span->context);
    if (return_code != 0) {
        /* Injection failed, log an error message. */
        fprintf(stderr, "Injection failed, return code = %d\n", return_code);
        return;
    }
```

#### Extract span context from an opentracing_text_map_reader

```c
    /* See text_map struct and more of its functions above. */

    /* Opaque iterator to iterate through entries in text_map. */
    typedef struct text_map_iterator text_map_iterator;

    /* Get a text_map_iterator from text_map. Returns valid iterator pointer
       on success, NULL on failure. */
    text_map_iterator* text_map_get_iterator(const text_map* map);

    /* Destroy a text_map_iterator returned from text_map_get_iterator. */
    void text_map_iterator_destroy(text_map_iterator* it);

    /* Returns opentracing_true if there are more entries to iterate through,
       opentracing_false otherwise. */
    opentracing_bool text_map_iterator_has_next(const text_map_iterator* it);

    /* Moves iterator to next entry, setting key pointer to next key and value
       pointer to next value. Undefined behavior if text_map_iterator_has_next
       returned opentracing_false (i.e. no valid entries left). */
    void text_map_iterator_next(
        text_map_iterator* it, const char** key_ptr, const char** value_ptr);

    typedef struct custom_carrier_reader {
        /* OPENTRACINGC_TEXT_MAP_READER_SUBCLASS expands to this function
           pointer. Not using macro for clarity. */
        opentracing_bool (*foreach_key)(opentracing_text_map_reader* reader,
                                        opentracing_bool (*f)(
                                            void*,
                                            const char*,
                                            const char*),
                                        void* arg);
        const text_map* map;
    } custom_carrier_reader;

    opentracing_bool custom_carrier_reader_foreach_key(
        opentracing_text_map_reader* reader,
        opentracing_bool (*f)(void*, const char*, const char*),
        void* arg)
    {
        custom_carrier_reader* r;
        text_map_iterator it;
        const char* key;
        const char* value;
        opentracing_bool success;

        assert(reader != NULL);
        assert(f != NULL);

        r = (custom_carrier_reader*) reader;
        it = text_map_get_iterator(r->map);
        if (it == NULL) {
            /* Failed to allocate iterator. */
            return opentracing_false;
        }

        for (; text_map_iterator_has_next(it);
             text_map_iterator_next(it, &key, &value)) {
            assert(key != NULL);
            assert(value != NULL);
            if (!f(arg, key, value)) {
                success = opentracing_false;
                goto cleanup;
            }
        }

        success = opentracing_true;

    cleanup:
        text_map_iterator_destroy(it);
        return success;
    }

    /* Initialize new reader with existing map. */
    void custom_carrier_reader_init(
        custom_carrier_reader* reader, const text_map* map)
    {
        assert(reader != NULL);
        reader->foreach_key = &custom_carrier_reader_foreach_key;
        reader->map = map;
    }
```
