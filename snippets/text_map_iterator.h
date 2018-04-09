#ifndef TEXT_MAP_ITERATOR_H
#define TEXT_MAP_ITERATOR_H

#include "text_map.h"

// tag::text-map-iterator-snippet
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
void text_map_iterator_next(text_map_iterator* it,
                            const char** key_ptr,
                            const char** value_ptr);
// end::text-map-iterator-snippet

#endif /* TEXT_MAP_ITERATOR_H */
