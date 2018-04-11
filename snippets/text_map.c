#include "text_map.h"

#include <stdlib.h>

struct text_map {
    int dummy;
};

opentracing_bool text_map_has_key(const text_map* map, const char* key)
{
    return opentracing_false;
}

opentracing_bool
text_map_insert(text_map* map, const char* key, const char* value)
{
    return opentracing_true;
}

text_map* text_map_new()
{
    text_map* map = malloc(sizeof(text_map));
    return map;
}

void text_map_destroy(text_map* map)
{
    free(map);
}
