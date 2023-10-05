#include "component_registry.h"

#include <stdlib.h>

static size_t *component_sizes = NULL;
static size_t num_registry_slots = 0;
static size_t num_registered_components = 0;

tECS_result_t register_component_type_s(size_t size, component_index_t *component_index_ptr) {

	if (!component_sizes) {
		// The registry has not been initialized. Therefore, initialize it.
		num_registry_slots = 8;
		component_sizes = (size_t *)calloc(num_registry_slots, sizeof(size_t));
		if (!component_sizes)
			return TECS_RESULT_BAD_ALLOC;
	}
	else if (num_registered_components >= num_registry_slots) {
		// The number of allocated slots has been filled, so eight more slots must be requested.
		// Eight slots are allocated at a time so that bitmask types, which must be a whole number of bytes, can be tested against the registry without dereferencing NULL pointers.
		size_t *new_ptr = (size_t *)realloc(component_sizes, (num_registry_slots += 8) * sizeof(size_t));
		if (new_ptr)
			component_sizes = new_ptr;
		else
			return TECS_RESULT_BAD_ALLOC;
	}
	component_sizes[num_registered_components] = size;

	if (component_index_ptr)
		*component_index_ptr = num_registered_components++;

	return TECS_RESULT_SUCCESS;
}

void get_component_sizes(component_mask_t component_mask, size_t *sizes) {
	size_t j = 0;
	for (size_t i = 0; i < (sizeof component_mask) * 8; ++i) {
		if ((component_mask >> i) & 1) {
			sizes[j++] = component_sizes[i];
		}
	}
}

size_t get_num_registered_components(void) {
	return num_registered_components;
}
