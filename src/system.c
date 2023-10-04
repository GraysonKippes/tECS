#include "system.h"

void execute_system(archetype_t *archetype_ptr, system_t system, void *data_ptr) {
	for (size_t i = 0; i < archetype_ptr->m_num_used_rows; ++i) {
		system(archetype_ptr, i, data_ptr);
	}
}
