#include <assert.h>
#include <string.h>

#include <opentracing-c/tracer.h>

static void null_destroy(opentracing_destructible* destructible)
{
    (void) destructible;
}

static opentracing_bool callback(void* arg, const char* key, const char* value)
{
    (void) key;
    (void) value;
    assert(arg != NULL);
    int* counter = (int*) arg;
    counter++;
    return opentracing_true;
}

typedef struct mock_text_map_reader {
    opentracing_text_map_reader base;
} mock_text_map_reader;

typedef struct mock_text_map_writer {
    opentracing_text_map_writer base;
} mock_text_map_writer;

typedef struct mock_http_headers_reader {
    opentracing_http_headers_reader base;
} mock_http_headers_reader;

typedef struct mock_http_headers_writer {
    opentracing_http_headers_writer base;
} mock_http_headers_writer;

typedef struct mock_custom_reader {
    opentracing_custom_carrier_reader base;
} mock_custom_reader;

typedef struct mock_custom_writer {
    opentracing_custom_carrier_writer base;
} mock_custom_writer;

static int
mock_binary_reader_callback(void* arg, char* buffer, size_t buffer_len)
{
    (void) arg;
    (void) buffer;
    (void) buffer_len;
    return 0;
}

static int mock_binary_write_callback(void* arg, const char* data, size_t len)
{
    (void) arg;
    (void) data;
    (void) len;
    return 0;
}

int main(void)
{
    opentracing_tracer* tracer;
    opentracing_span* span;
    opentracing_span_context* span_context;
    int return_code;
    opentracing_value value;
    opentracing_tracer* global_tracer;
    opentracing_tracer dummy_tracer;
    int counter;
    mock_text_map_reader text_map_reader;
    mock_text_map_writer text_map_writer;
    mock_http_headers_reader http_headers_reader;
    mock_http_headers_writer http_headers_writer;
    mock_custom_reader custom_reader;
    mock_custom_writer custom_writer;

    memset(&text_map_reader, 0, sizeof(text_map_reader));
    memset(&text_map_writer, 0, sizeof(text_map_writer));
    memset(&http_headers_reader, 0, sizeof(http_headers_reader));
    memset(&http_headers_writer, 0, sizeof(http_headers_writer));
    memset(&custom_reader, 0, sizeof(custom_reader));
    memset(&custom_writer, 0, sizeof(custom_writer));

    tracer = opentracing_global_tracer();
    span = tracer->start_span(tracer, "a");
    assert(span != NULL);
    span_context = span->span_context(span);
    assert(span_context != NULL);
    counter = 0;
    span_context->foreach_baggage_item(
        span_context, &callback, (void*) &counter);
    assert(counter == 0);
    assert(span->tracer(span) == tracer);
    assert(strlen(span->baggage_item(span, "key")) == 0);
    span->set_baggage_item(span, "key", "value");
    span->set_tag(span, "tag", &value);
    span->set_operation_name(span, "operation");
    span->log_fields(span, NULL, 0);

    span->finish(span);
    ((opentracing_destructible*) span)
        ->destroy((opentracing_destructible*) span);

    return_code = tracer->extract_text_map(
        tracer, (opentracing_text_map_reader*) &text_map_reader, &span_context);
    assert(return_code == 0);
    assert(span_context != NULL);
    return_code = tracer->inject_text_map(
        tracer, (opentracing_text_map_writer*) &text_map_writer, span_context);
    ((opentracing_destructible*) span_context)
        ->destroy((opentracing_destructible*) span_context);

    return_code = tracer->extract_http_headers(
        tracer,
        (opentracing_http_headers_reader*) &http_headers_reader,
        &span_context);
    assert(return_code == 0);
    assert(span_context != NULL);
    return_code = tracer->inject_http_headers(
        tracer,
        (opentracing_http_headers_writer*) &http_headers_writer,
        span_context);
    ((opentracing_destructible*) span_context)
        ->destroy((opentracing_destructible*) span_context);

    return_code = tracer->extract_custom(
        tracer,
        (opentracing_custom_carrier_reader*) &custom_reader,
        &span_context);
    assert(return_code == 0);
    assert(span_context != NULL);
    return_code = tracer->inject_custom(
        tracer,
        (opentracing_custom_carrier_writer*) &custom_writer,
        span_context);
    ((opentracing_destructible*) span_context)
        ->destroy((opentracing_destructible*) span_context);

    return_code = tracer->extract_binary(
        tracer, &mock_binary_reader_callback, NULL, &span_context);
    assert(return_code == 0);
    assert(span_context != NULL);
    return_code = tracer->inject_binary(
        tracer, &mock_binary_write_callback, NULL, span_context);
    ((opentracing_destructible*) span_context)
        ->destroy((opentracing_destructible*) span_context);

    opentracing_init_global_tracer(opentracing_global_tracer());

    global_tracer = opentracing_global_tracer();
    memset(&dummy_tracer, 0, sizeof(dummy_tracer));
    dummy_tracer.base.destroy = &null_destroy;
    opentracing_init_global_tracer(&dummy_tracer);
    assert(opentracing_global_tracer() == &dummy_tracer);
    opentracing_init_global_tracer(global_tracer);
    assert(opentracing_global_tracer() == global_tracer);

    global_tracer->close(global_tracer);

    return 0;
}
