/*
 * Loop table entry structure
 */
#ifndef LTBL_H
#define LTBL_H

#include "ripte_common.h"

typedef struct ltbl_entry {
	ripte_addr_t entry;
	ripte_addr_t exit;
} ltbl_entry_t;



#endif /* LTBL_H */

