#include "entity_manager.h"

#include <stddef.h>

#include "record.h"

/* -- ENTITY MANAGEMENT -- */

// Array that stores all entities.
static entity_t entities[MAX_NUM_ENTITIES];

// Array that stores all entities' positions.
// Using the entity itself as an offset into this array, there is found the position of that entity in entities.
static size_t entity_positions[MAX_NUM_ENTITIES];

// Number of currently allocated entities.
// Each time an entity is allocated/borrowed, this number is incremented; each time an entity is freed, this number is decremented.
// Acts as a binary partition; every entity before entities[num_allocated_entities] is unavailable, and every entity at and after entities[num_allocated_entities] is available. The entity directly at entities[num_allocated_entities] is the first available entity (FAE).
static size_t num_allocated_entities;

/* -- RECORD MAPPING -- */

// Array that stores all records corresponding to active entity-handles.
// Using an entity as an index into this array retrieves that entity's record.
static record_t entity_records[MAX_NUM_ENTITIES];

/* -- FUNCTION DEFINITIONS -- */

void init_entity_manager(void) {
	for (size_t i = 0; i < MAX_NUM_ENTITIES; ++i) {
		entities[i] = (entity_t)i;
		entity_positions[i] = i;
	}
	num_allocated_entities = 0;
}

TECS_result_t create_entity(archetype_t *archetype_ptr, entity_t *entity_ptr) {
	
	if (num_allocated_entities >= MAX_NUM_ENTITIES)
		return TECS_RESULT_NO_ENTITIES_AVAILABLE;

	// Swap the entity at the back with the first available entity.
	entity_t swap = entities[num_allocated_entities];	// This is the first available entity and it will be returned.
	entities[num_allocated_entities] = entities[MAX_NUM_ENTITIES - 1]; // Put the back entity where the gotten entity is, which will be the last unavailable position.
	entity_positions[entities[MAX_NUM_ENTITIES - 1]] = num_allocated_entities;	// The back entity's new position is at num_allocated_entities. 
	entities[MAX_NUM_ENTITIES - 1] = swap;	// Put the gotten entity at the back.
	entity_positions[swap] = MAX_NUM_ENTITIES - 1;	// The gotten entity's new position is at the back.
	num_allocated_entities++;

	// Create a new record for this entity.
	entity_records[swap].m_archetype_ptr = archetype_ptr;
	entity_records[swap].m_row = archetype_ptr->m_num_used_rows;
	archetype_add_row(archetype_ptr, swap);

	if (entity_ptr)
		*entity_ptr = swap;

	return TECS_RESULT_SUCCESS;
}

record_t get_entity_record(entity_t entity) {
	return entity_records[entity];
}

TECS_result_t free_entity(entity_t entity) {

	if (entity >= MAX_NUM_ENTITIES)
		return TECS_RESULT_INVALID_ENTITY_ID;

	if (entity_positions[entity] >= num_allocated_entities)
		return TECS_RESULT_ENTITY_ALREADY_FREE;

	// Remove the entity's corresponding row in its archetype, then clear its record.
	archetype_t *archetype_ptr = entity_records[entity].m_archetype_ptr;
	size_t row = entity_records[entity].m_row;
	archetype_remove_row(archetype_ptr, row);
	// If a middle row was removed, then the back row was moved into its spot, and the corresponding record must be updated to reflect that.
	if (row == archetype_ptr->m_num_used_rows)
		entity_records[archetype_ptr->m_rows_to_entities[row]].m_row = row;
	entity_records[entity].m_archetype_ptr = NULL;
	entity_records[entity].m_row = 0;

	// Swap the freed entity with the last unavailable entity.
	// When num_allocated_entities is decremented, that position is exposed, becoming the first available entity.
	entities[entity_positions[entity]] = entities[--num_allocated_entities];	// Get the position of the entity being freed, and set that position in entities to the last unavailable entity. Expose the last unavailable position.
	entity_positions[entities[num_allocated_entities]] = entity_positions[entity];	// The last unavailable entity's position is now the freed entity's old position.
	entities[num_allocated_entities] = entity;	// Set the first available entity to the freed entity.
	entity_positions[entity] = num_allocated_entities;	// Record the freed entity's new position.
	
	return TECS_RESULT_SUCCESS;
}
