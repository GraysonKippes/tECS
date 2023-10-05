#ifndef TECS_H
#define TECS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif



/* -- USER-DEFINABLE TYPES -- */

#ifndef ENTITY_TYPE
#define ENTITY_TYPE unsigned int
#endif

// An entity is a numerical ID that acts as a handle into a set of components.
typedef ENTITY_TYPE entity_t;

#ifndef COMPONENT_MASK_TYPE
#define COMPONENT_MASK_TYPE uint8_t
#endif

// A component mask is a signature of which components an entity does and does not have.
// Each bit corresponds to a component index; if a bit is 0, then the entity does not have the corresponding compoent, and if a bit is 1, then the entity does have the corresponding component.
typedef COMPONENT_MASK_TYPE component_mask_t;



/* -- PRE-DEFINED TYPES -- */

// A tECS result is a status code that is returned from certain functions, indicating whether the function was performed successfully or not.
typedef enum tECS_result_t {
	TECS_RESULT_SUCCESS = 0,
	TECS_RESULT_BAD_ALLOC,
	TECS_RESULT_NO_ENTITIES_AVAILABLE,
	TECS_RESULT_INVALID_ENTITY_ID,
	TECS_RESULT_ENTITY_ALREADY_FREE,
	TECS_RESULT_COMPONENT_TABLE_ROW_REMOVE_OUT_OF_BOUNDS
} tECS_result_t;

// A component index is an unsigned integer type used for indexing component-types.
typedef size_t component_index_t;

// A component array is a dynamically-resizing array of components of the same type.
typedef struct component_array_t {
	
	// Internal pointer-array of components.
	void *m_components;

	// Size of each component in bytes.
	size_t m_component_size;

} component_array_t;

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

// A record of an entity indicates what archetype that entity belongs to, and in which row that entity's components can be found.
typedef struct record_t {

	// Points to the archetype to which the entity to which this record belongs belongs.
	archetype_t *m_archetype_ptr;

	// The row in the archetype corresponding to the entity to which this record belongs.
	size_t m_row;

} record_t;

// A system is a function that can be executed on all rows of components in an archetype.
typedef void (*system_t)(archetype_t *archetype_ptr, size_t row, void *data_ptr);



/* -- FUNCTION DECLARATIONS -- */

/*	Component Functions */

// Registers a component-type of the given size.
// If parameter component_index_ptr is not null, then it is set to the index of the new component-type.
// Returns TECS_RESULT_BAD_ALLOC if (re)allocation occurs but fails.
tECS_result_t register_component_type_s(size_t size, component_index_t *component_index_ptr);

// Macro for registering a component-type directly from the typename.
#define register_component_type(type, index_ptr) (register_component_type_s(sizeof(type), index_ptr))

// Populates the given pointer-array of sizes with the sizes of the registered components corresponding to the 1s in the bitmask.
// It is up to the caller to create a pointer/array with enough slots to fit all the sizes.
void get_component_sizes(component_mask_t component_mask, size_t *sizes);

// Returns the total number of components registered by the user.
size_t get_num_registered_components(void);

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

/*	Archetype Functions */

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

/*	Entitiy Functions */

// Initializes the entity manager, preparing the pool of available entities.
void init_entity_manager(void);

// Creates a new entity at location *entity_ptr, belonging to the archetype at location *archetype_ptr.
// Returns TECS_RESULT_NO_ENTITY_AVAILABLE if no more entities are available.
tECS_result_t create_entity(archetype_t *archetype_ptr, entity_t *entity_ptr);

// Return this entity's record.
record_t get_entity_record(entity_t entity);

// Macro that calls get_entity_component and casts the return value to a pointer to data of the specified type.
#define get_entity_component(component_type, entity, component_index)\
	(component_array_get(component_type, archetype_get_column(get_entity_record(entity).m_archetype_ptr, component_index), get_entity_record(entity).m_row))

// Destroys an entity, returning the handle to the pool of available entities.
// Returns TECS_RESULT_INVALID_ENTITY_ID if the given entity ID is not a valid entity ID.
// Returns TECS_RESULT_ENTITY_ALREADY_FREE if the given entity is already free.
tECS_result_t free_entity(entity_t entity);

/*	System Functions */

// Executes the system on the archetype.
void execute_system(archetype_t *archetype_ptr, system_t system, void *data_ptr);

#ifdef __cplusplus
}
#endif

#endif  // TECS_H
