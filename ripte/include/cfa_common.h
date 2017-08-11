#ifndef RIPTE_COMMON_H
#define RIPTE_COMMON_H

/* Length of the computed digests */
#define DIGEST_SIZE_BYTES       16

/* Length of instruction in bytes */
#define INSTRUCTION_LEN		4

/* RIPTE event types */
#define RIPTE_EVENT_INIT		 0x00000000
#define RIPTE_EVENT_B		 0x00000001
#define RIPTE_EVENT_BL		 0x00000002
#define RIPTE_EVENT_BX_LR		 0x00000004
#define RIPTE_EVENT_POP_FP_PC	 0x00000008
#define RIPTE_EVENT_POP_FP_LR	 0x00000010
#define RIPTE_EVENT_BLX_R3	 0x00000020
#define RIPTE_EVENT_QUOTE		 0x80000000
#define RIPTE_EVENT_ERROR		 0x000000FF

/* Internal RIPTE return values */
#define RIPTE_SUCCESS              0x00000000
#define RIPTE_ERROR_GENERIC        0xFFFF0000
#define RIPTE_ERROR_BAD_PARAMETERS 0xFFFF0006
#define RIPTE_ERROR_OUT_OF_MEMORY  0xFFFF000C

#ifndef ASSEMBLY

#include <stdint.h>

/* Type alias for memory address */
typedef uint32_t ripte_addr_t;

/* Structure for transferring init parameters */
typedef struct ripte_init_params {
	ripte_addr_t main_start;
	ripte_addr_t main_end;
	const struct btbl_entry *btbl_start;
	const struct btbl_entry *btbl_end;
	const struct ltbl_entry *ltbl_start;
	const struct ltbl_entry *ltbl_end;
} ripte_init_params_t;

/* Structure for transferring quote parameters */
typedef struct ripte_quote_params {
	const uint8_t *user_data;
	uint32_t user_data_len;
	uint8_t *out;
	uint32_t *out_len;
} ripte_quote_params_t;

#endif

#endif /* RIPTE_COMMON_H */

