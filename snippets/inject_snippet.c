typedef struct text_map_writer {
    /* Base class instance. */
    opentracing_text_map_writer base;

    /* Map object. */
    text_map* map;
} text_map_writer;

static void noop_destroy(opentracing_destructible* d)
{
    /* noop */
}

static opentracing_propagation_error_code text_map_writer_set(
    opentracing_text_map_writer* writer, const char* key, const char* value)
{
    text_map_writer* w;

    assert(writer != NULL);
    assert(key != NULL);
    assert(value != NULL);

    w = (text_map_writer*) writer;
    if (text_map_has_key(w->map, key) || !text_map_insert(w->map, key, value)) {
        return opentracing_propagation_error_code_unknown;
    }
    return opentracing_propagation_error_code_success;
}

opentracing_bool text_map_writer_init(text_map_writer* writer)
{
    assert(writer != NULL);
    ((opentracing_destructible*) writer)->destroy = &noop_destroy;
    ((opentracing_text_map_writer*) writer)->set = &text_map_writer_set;
    writer->map = text_map_new();
    return (writer->map != NULL) ? opentracing_true : opentracing_false;
}

void inject(void)
{
    text_map_writer writer;
    opentracing_tracer* tracer;
    opentracing_span* span;
    opentracing_propagation_error_code return_code;
    if (!text_map_writer_init(&writer)) {
        return;
    }
    tracer = opentracing_global_tracer();
    span = tracer->start_span(tracer, "test-inject");
    return_code =
        tracer->inject_text_map(tracer,
                                (opentracing_text_map_writer*) &writer,
                                span->span_context(span));
    if (return_code != 0) {
        /* Injection failed, log an error message. */
        fprintf(stderr, "Injection failed, return code = %d\n", return_code);
        return;
    }
}
