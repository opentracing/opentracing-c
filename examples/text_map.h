#ifndef TEXT_MAP_H
#define TEXT_MAP_H

#include <opentracing-c/tracer.h>

/* Custom map implementation. */
typedef struct text_map text_map;
/* Returns opentracing_true if map initialization succeeded, false
   otherwise. */
/* Returns opentracing_true if map has key, opentracing_false otherwise. */
opentracing_bool text_map_has_key(const text_map* map, const char* key);
/* Returns opentracing_true if insertion succeeded, opentracing_false
   otherwise. */
opentracing_bool
text_map_insert(text_map* map, const char* key, const char* value);

/* Returns a new text_map if allocation and initialization succeeded, NULL
   otherwise. */
text_map* text_map_new(void);

void text_map_destroy(text_map* map);

#endif /* TEXT_MAP_H */
