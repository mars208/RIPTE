#ifndef RIPTE_H
#define RIPTE_H

#ifdef RIPTE_STUB_H
#error "RIPTE_STUB_H already defined, did you mean to call the normal world API?"
#endif

#include <stdint.h>
#include <stdbool.h>

#include "list.h"

#include "btbl.h"
#include "ltbl.h"

#include "ripte_common.h"
#include "ripte_private.h"

#include "tee_api.h"

typedef struct ripte_event {
	uint32_t type;
	ripte_addr_t src_addr;
	ripte_addr_t dst_addr;
	ripte_addr_t lr_value;
} ripte_event_t;

/* Node in the control flow graph */
typedef struct ripte_node {
	ripte_addr_t start;
	ripte_addr_t end;
} ripte_node_t;

/* Path descriptor */
typedef struct ripte_path_desc {
	uint32_t ctr;
	uint8_t digest[DIGEST_SIZE_BYTES];
} ripte_path_desc_t;

/* Currently executed path context */
typedef struct ripte_path_ctx {
	ripte_hash_state_t state;
} ripte_path_ctx_t;

/* Loop descriptor */
typedef struct ripte_loop_desc {
	uint8_t digest[DIGEST_SIZE_BYTES];
	list_t path_list;
} ripte_loop_desc_t;

/* Currently executed loop context */
typedef struct ripte_loop_ctx {
	ripte_node_t entry;
	ripte_addr_t exit;
	ripte_addr_t ret;
	ripte_path_ctx_t *path;
	ripte_loop_desc_t *desc;
} ripte_loop_ctx_t;

/* Context for ripte operations */
typedef struct ripte_ctx {
	ripte_hash_state_t state;
	ripte_node_t cur_node;
	ripte_addr_t main_start;
	ripte_addr_t main_end;
	const btbl_entry_t *btbl_start;
	const btbl_entry_t *btbl_end;
	const ltbl_entry_t *ltbl_start;
	const ltbl_entry_t *ltbl_end;
	list_t loop_stack;
	list_t loop_list;
	bool initialized;
} ripte_ctx_t;

/* Secure world API */

/*!
 * \brief ripte_get_ctx
 * Returns the current ripte context.
 */
ripte_ctx_t* ripte_get_ctx(void);

/*!
 * \brief ripte_init
 * Initialize the ripte subsystem.
 * \param main_start Start address of main function
 * \param main_end End address of main function
 * \param btbl_start Start address of branch table
 * \param btbl_end End address of branch table
 */
uint32_t ripte_init(ripte_ctx_t *ctx,
		  const ripte_addr_t main_start, const ripte_addr_t main_end,
		  const btbl_entry_t *btbl_start, const btbl_entry_t *btbl_end,
		  const ltbl_entry_t *ltbl_start, const ltbl_entry_t *ltbl_end);

/*!
 * \brief ripte_event
 * Extend the digest.
 * \param type Type of control flow event (e.g. ripte_EVENT_XX)
 * \param src_addr Source address of the control flow event
 * \param dst_addr Destination address of the control flow event
 */
uint32_t ripte_event(ripte_ctx_t *ctx, const ripte_event_t *event);

/*!
 * \brief ripte_quote
 * Quote the current digest value.
 * \param user_data Unpredictable user-supplied data to be included in quote
 * \param user_data_len Length of the user-supplied data
 * \param out Output buffer for storing the digest quote
 * \param outlen Length of the output buffer for storing the digest quote
 */
uint32_t ripte_quote(ripte_ctx_t *ctx,
		   const uint8_t *user_data, const uint32_t user_data_len,
		   uint8_t *out, uint32_t *out_len);

#endif /* ripte_H */
