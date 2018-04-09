#ifndef INJECT_H
#define INJECT_H

#include <opentracing-c/tracer.h>

struct text_map;

void extract(const struct text_map* map);

#endif /* INJECT_H */
