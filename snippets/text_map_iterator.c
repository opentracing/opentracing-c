#include "text_map_iterator.h"

#include <stdlib.h>

struct text_map_iterator {
    int dummy;
};

text_map_iterator* text_map_get_iterator(const text_map* map)
{
    return malloc(sizeof(text_map_iterator));
}

void text_map_iterator_destroy(text_map_iterator* it)
{
    free(it);
}

opentracing_bool text_map_iterator_has_next(const text_map_iterator* it)
{
    return opentracing_false;
}

void text_map_iterator_next(text_map_iterator* it,
                            const char** key_ptr,
                            const char** value_ptr)
{
}
