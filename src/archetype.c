#include "archetype.h"

#include <stdlib.h>
#include <string.h>

TECS_result_t create_archetype(const component_mask_t component_mask, archetype_t *archetype_ptr) {

	if (!archetype_ptr)
		return TECS_RESULT_SUCCESS;

	// Find the number of columns (component arrays) from the number of 1s in the bitmask.
	size_t num_component_arrays = 0;
	size_t *component_indices_to_columns = calloc(get_num_registered_components(), sizeof(size_t));
	if (!component_indices_to_columns)
		return TECS_RESULT_BAD_ALLOC;

	for (size_t i = 0; i < sizeof(component_mask_t) * 8; ++i) {
		size_t is_bit_1 = (component_mask >> i) & 1;
		// Due to byte-alignment, the component mask likely has more bits than the actual number of registered components, so check if `i` is still within the latter bounds.
		if (i < get_num_registered_components() && is_bit_1)
			component_indices_to_columns[i] = num_component_arrays;
		num_component_arrays += is_bit_1;
	}

	archetype_ptr->m_component_mask = component_mask;
	archetype_ptr->m_num_columns = num_component_arrays;
	archetype_ptr->m_component_indices_to_columns = component_indices_to_columns;

	// Create column members.
	size_t sizes[archetype_ptr->m_num_columns];
	get_component_sizes(archetype_ptr->m_component_mask, sizes);

	// Allocate that number of columns and initialize each column.
	archetype_ptr->m_component_table = malloc(archetype_ptr->m_num_columns * sizeof(component_array_t));
	if (!archetype_ptr->m_component_table)
		return TECS_RESULT_BAD_ALLOC;

	for (size_t i = 0; i < archetype_ptr->m_num_columns; ++i) {
		TECS_result_t result = create_component_array(sizes[i], archetype_ptr->m_component_table + i);
		if (result != TECS_RESULT_SUCCESS)
			return result;
	}

	// Create row members.
	archetype_ptr->m_num_rows = COMPONENT_ARRAY_INITIAL_COUNT;
	archetype_ptr->m_num_used_rows = 0;
	archetype_ptr->m_rows_to_entities = NULL;

	return TECS_RESULT_SUCCESS;
}

component_array_t archetype_get_column(archetype_t *archetype_ptr, component_index_t component_index) {
	return archetype_ptr->m_component_table[archetype_ptr->m_component_indices_to_columns[component_index]];
}

TECS_result_t archetype_add_row(archetype_t *archetype_ptr, entity_t entity) {

	// If there are no more emtpy rows left, then allocate a new row.
	if (archetype_ptr->m_num_used_rows >= archetype_ptr->m_num_rows) {
		archetype_ptr->m_num_rows++;
		for (size_t i = 0; i < archetype_ptr->m_num_columns; ++i) {
			component_array_resize(archetype_ptr->m_component_table + i, archetype_ptr->m_num_rows);
		}
	}

	// Increment used rows counter and map the row to the entity.
	archetype_ptr->m_num_used_rows++;
	archetype_ptr->m_rows_to_entities = realloc(archetype_ptr->m_rows_to_entities, archetype_ptr->m_num_used_rows * sizeof(entity_t));
	archetype_ptr->m_rows_to_entities[archetype_ptr->m_num_used_rows - 1] = entity;

	return TECS_RESULT_SUCCESS;
}

TECS_result_t archetype_remove_row(archetype_t *archetype_ptr, size_t row) {

	if (row >= archetype_ptr->m_num_used_rows)
		return TECS_RESULT_COMPONENT_TABLE_ROW_REMOVE_OUT_OF_BOUNDS;

	// If there is only one row, then no reallocation can or must be performed.
	if (archetype_ptr->m_num_used_rows == 1) {
		archetype_ptr->m_num_used_rows = 0;
		return TECS_RESULT_SUCCESS;
	}

	// If the row is not at the end of the table, replace it with the back row first before removing the back.
	// In this case, parameter row will now index into the row that was moved.
	if (row < archetype_ptr->m_num_used_rows - 1) {
		for (size_t i = 0; i < archetype_ptr->m_num_columns; ++i) {
			// Copy the bytes of the back component into the component to be removed.
			size_t component_size = archetype_ptr->m_component_table[i].m_component_size;
			void *dest = (unsigned char *)archetype_ptr->m_component_table[i].m_components + (row * component_size);
			void *src = (unsigned char *)archetype_ptr->m_component_table[i].m_components + ((archetype_ptr->m_num_used_rows - 1) * component_size);
			memcpy(dest, src, component_size);
		}
		archetype_ptr->m_rows_to_entities[row] = archetype_ptr->m_rows_to_entities[archetype_ptr->m_num_used_rows - 1];
	}

	// Remove the last row.
	archetype_ptr->m_num_rows--;
	archetype_ptr->m_num_used_rows--;
	for (size_t i = 0; i < archetype_ptr->m_num_columns; ++i) {
		component_array_resize(archetype_ptr->m_component_table + i, archetype_ptr->m_num_rows);
	}
	archetype_ptr->m_rows_to_entities = realloc(archetype_ptr->m_rows_to_entities, archetype_ptr->m_num_used_rows * sizeof(entity_t));

	return TECS_RESULT_SUCCESS;
}

void free_archetype(archetype_t archetype) {

	// Free all individual columns (component arrays).
	for (size_t i = 0; i < archetype.m_num_columns; ++i) {
		free_component_array(archetype.m_component_table[i]);
	}

	free(archetype.m_component_table);
	free(archetype.m_component_indices_to_columns);
	free(archetype.m_rows_to_entities);
}
