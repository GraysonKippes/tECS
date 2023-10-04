#ifndef RECORD_H
#define RECORD_H

#include <stddef.h>

#include "entity.h"
#include "archetype.h"

typedef struct record_t {
	archetype_t *m_archetype_ptr;
	size_t m_row;
} record_t;

// Returns a pointer to the component at the specified column in the record's row in the record's component table.
void *get_component(record_t record, size_t column);

#endif // RECORD_H
