#ifndef SYSTEM_H
#define SYSTEM_H

#include "archetype.h"

// Pointer to a function that can be executed on all rows of an archetype.
typedef void (*system_t)(archetype_t *archetype_ptr, size_t row, void *data_ptr);

// Executes the system on the archetype.
void execute_system(archetype_t *archetype_ptr, system_t system, void *data_ptr);

#endif // SYSTEM_H
