#include "component.h"

#include <stdlib.h>

TECS_result_t create_component_array(size_t component_size, component_array_t *component_array_ptr) {
	if (component_array_ptr) {
		component_array_ptr->m_component_size = component_size;
		component_array_ptr->m_components = calloc(COMPONENT_ARRAY_INITIAL_COUNT, component_array_ptr->m_component_size);
		if (!component_array_ptr->m_components)
			return TECS_RESULT_BAD_ALLOC;
	}
	return TECS_RESULT_SUCCESS;
}

TECS_result_t component_array_resize(component_array_t *component_array_ptr, size_t new_count) {
	void *new_ptr = realloc(component_array_ptr->m_components, new_count * component_array_ptr->m_component_size);
	if (new_ptr)
		component_array_ptr->m_components = new_ptr;
	else
		return TECS_RESULT_BAD_ALLOC;
	return TECS_RESULT_SUCCESS;
}

void free_component_array(component_array_t component_array) {
	free(component_array.m_components);
}
