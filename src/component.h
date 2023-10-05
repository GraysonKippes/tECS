#ifndef COMPONENT_H
#define COMPONENT_H

#include <stddef.h>
#include <stdint.h>

#include "tecs_result.h"

#ifndef COMPONENT_ARRAY_INITIAL_COUNT
#define COMPONENT_ARRAY_INITIAL_COUNT	8
#endif

// A component array is a dynamically-resizing array of components of the same type.
typedef struct component_array_t {
	
	// Internal pointer-array of components.
	void *m_components;

	// Size of each component in bytes.
	size_t m_component_size;

} component_array_t;

// Sets *component_array_ptr to a new component array, with the speicified component size.
// If parameter component_array_ptr is null, then this function does nothing and silently returns TECS_RESULT_SUCCESS.
tECS_result_t create_component_array(size_t component_size, component_array_t *component_array_ptr);

// Resizes the component array with the specified count.
// Returns TECS_RESULT_BAD_ALLOC if reallocation failed.
tECS_result_t component_array_resize(component_array_t *component_array_ptr, size_t new_count);

// Returns a pointer to the component of the specified type at the given index.
// The component array must be passed in directly, and not by pointer.
#define component_array_get(component_type, component_array, index) ((component_type *)component_array.m_components + index) 

// Destroys the component array, freeing the internal pointer.
void free_component_array(component_array_t component_array);

#endif	// COMPONENT_H
