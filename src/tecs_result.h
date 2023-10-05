#ifndef TECS_RESULT_H
#define TECS_RESULT_H

// A tECS result is a status code that is returned from certain functions, indicating whether the function was performed successfully or not.
typedef enum tECS_result_t {
	TECS_RESULT_SUCCESS = 0,
	TECS_RESULT_BAD_ALLOC,
	TECS_RESULT_NO_ENTITIES_AVAILABLE,
	TECS_RESULT_INVALID_ENTITY_ID,
	TECS_RESULT_ENTITY_ALREADY_FREE,
	TECS_RESULT_COMPONENT_TABLE_ROW_REMOVE_OUT_OF_BOUNDS
} tECS_result_t;

#endif // TECS_RESULT_H
