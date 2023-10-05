#ifndef RECORD_H
#define RECORD_H

#include <stddef.h>

#include "entity.h"
#include "archetype.h"

// A record of an entity indicates what archetype that entity belongs to, and in which row that entity's components can be found.
typedef struct record_t {

	// Points to the archetype to which the entity to which this record belongs belongs.
	archetype_t *m_archetype_ptr;

	// The row in the archetype corresponding to the entity to which this record belongs.
	size_t m_row;

} record_t;

#endif // RECORD_H
