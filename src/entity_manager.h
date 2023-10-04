#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <limits.h>

#include "tecs_result.h"
#include "entity.h"
#include "component.h"
#include "archetype.h"
#include "record.h"

#ifndef MAX_NUM_ENTITIES
#define MAX_NUM_ENTITIES 256
#endif

// Initializes the entity manager, preparing the pool of available entities.
void init_entity_manager(void);

// Creates a new entity at location *entity_ptr, belonging to the archetype at location *archetype_ptr.
// Returns TECS_RESULT_NO_ENTITY_AVAILABLE if no more entities are available.
TECS_result_t create_entity(archetype_t *archetype_ptr, entity_t *entity_ptr);

// Return this entity's record.
record_t get_entity_record(entity_t entity);

// Macro that calls get_entity_component and casts the return value to a pointer to data of the specified type.
#define get_entity_component(component_type, entity, component_index)\
	(component_array_get(component_type, archetype_get_column(get_entity_record(entity).m_archetype_ptr, component_index), get_entity_record(entity).m_row))

// Destroys an entity, returning the handle to the pool of available entities.
// Returns TECS_RESULT_INVALID_ENTITY_ID if the given entity ID is not a valid entity ID.
// Returns TECS_RESULT_ENTITY_ALREADY_FREE if the given entity is already free.
TECS_result_t free_entity(entity_t entity);

#endif	// ENTITY_MANAGER_H
