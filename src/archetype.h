#ifndef ARCHETYPE_H
#define ARCHETYPE_H

#include <stddef.h>

#include "tecs_result.h"
#include "entity.h"
#include "component.h"
#include "component_registry.h"

// An archetype is a table of all components belonging to entities with the same signature, or component mask.
typedef struct archetype_t {

	// The signature of this archetype, or the signature of the entities to which the components in this archetype belong.
	component_mask_t m_component_mask;

	// The component table of this archetype, stores all components.
	component_array_t *m_component_table;

	// Number of columns in this archetype's component table. Unlike the number of rows, this value is fixed at archetype creation.
	size_t m_num_columns;

	// Maps component-type indices to columns in this archetype.
	// The count of this pointer-array is guaranteed to be equal to the number of registered components.
	component_index_t *m_component_indices_to_columns;

	// Number of rows allocated in this archetype's component table.
	size_t m_num_rows;

	// Number of rows currently being used in this archetype's component table.
	size_t m_num_used_rows;

	// Maps row indices to entities.
	// This is used when removing a row in the middle of the table and the back row has to be moved to that position.
	entity_t *m_rows_to_entities;

} archetype_t;

// Creates a new archetype with the specified signature.
// Automagically creates the appropriate number of component arrays as columns in the component table.
tECS_result_t create_archetype(const component_mask_t component_mask, archetype_t *archetype_ptr);

// Returns the archetype column of the component-type indicated by the index.
// If no such column exists, then the first column is returned.
component_array_t archetype_get_column(archetype_t *archetype_ptr, component_index_t component_index);

// Adds a new row at the end of the archetype's component table.
tECS_result_t archetype_add_row(archetype_t *archetype_ptr, entity_t entity);

// Removes a row from the archetype's component table.
tECS_result_t archetype_remove_row(archetype_t *archetype_ptr, size_t row);

// Deletes the archetype, freeing all pointers.
void free_archetype(archetype_t archetype);

#endif	// ARCHETYPE_H
