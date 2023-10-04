#ifndef COMPONENT_REGISTRY_H
#define COMPONENT_REGISTRY_H

#include <stddef.h>

#include "tecs_result.h"
#include "component.h"

#ifndef COMPONENT_MASK_TYPE
#define COMPONENT_MASK_TYPE uint8_t
#endif

// component_mask_t is a signature of which components an entity does and does not have.
// Each bit corresponds to a component index; if a bit is 0, then the entity does not have the corresponding compoent, and if a bit is 1, then the entity does have the corresponding component.
typedef COMPONENT_MASK_TYPE component_mask_t; 

// component_index_t is an unsigned integer type used for indexing component-types.
typedef size_t component_index_t;

// Registers a component-type of the given size.
// If parameter component_index_ptr is not null, then it is set to the index of the new component-type.
// Returns TECS_RESULT_BAD_ALLOC if (re)allocation occurs but fails.
TECS_result_t register_component_type_s(size_t size, component_index_t *component_index_ptr);

// Macro for registering a component-type directly from the typename.
#define register_component_type(type, index_ptr) (register_component_type_s(sizeof(type), index_ptr))

// Populates the given pointer-array of sizes with the sizes of the registered components corresponding to the 1s in the bitmask.
// It is up to the caller to create a pointer/array with enough slots to fit all the sizes.
void get_component_sizes(component_mask_t component_mask, size_t *sizes);

// Returns the total number of components registered by the user.
size_t get_num_registered_components(void);

#endif	// COMPONENT_REGISTRY_H
