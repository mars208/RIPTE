
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

#include "tee_api.h"

#include "blake2.h"

typedef blake2s_state ripte_hash_state_t;

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


typedef struct ftbl_entry {
	uint32_t src;
	uint32_t dst;
	uint8_t lr[64];
	uint8_t type;
} ftbl_entry_t;

/* Context for RIPTE operations */
typedef struct ripte_ctx {
	ripte_hash_state_t state;
	ripte_node_t cur_node;
	ripte_addr_t main_start;
	ripte_addr_t main_end;
	const btbl_entry_t *btbl_start;
	const btbl_entry_t *btbl_end;
	const ltbl_entry_t *ltbl_start;
	const ltbl_entry_t *ltbl_end;
	ftbl_entry_t *ftbl_start;
	ftbl_entry_t *ftbl_end;
	list_t loop_stack;
	list_t loop_list;
	bool initialized;
} ripte_ctx_t;

struct rc4_state {
    uint8_t  perm[256];
    uint8_t  index1;
    uint8_t  index2;
};

void rc4_init(struct rc4_state *state, const uint8_t *key, int keylen);
void rc4_crypt(struct rc4_state *state, const uint8_t *inbuf, uint8_t *outbuf, int buflen);

/* Secure world API */

/*!
 * \brief ripte_get_ctx
 * Returns the current RIPTE context.
 */
ripte_ctx_t* ripte_get_ctx(void);

/*!
 * \brief ripte_init
 * Initialize the RIPTE subsystem.
 * \param main_start Start address of main function
 * \param main_end End address of main function
 * \param btbl_start Start address of branch table
 * \param btbl_end End address of branch table
 */
uint32_t ripte_init(ripte_ctx_t *ctx,
		  const ripte_addr_t main_start, const ripte_addr_t main_end,
		  const btbl_entry_t *btbl_start, const btbl_entry_t *btbl_end,
		  const ltbl_entry_t *ltbl_start, const ltbl_entry_t *ltbl_end,
		  ftbl_entry_t* ftbl_start, ftbl_entry_t* ftbl_end);

/*!
 * \brief ripte_event
 * Extend the digest.
 * \param type Type of control flow event (e.g. RIPTE_EVENT_XX)
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


ripte_addr_t decrypt(uint32_t lr_value, ripte_addr_t key, ripte_addr_t measurement);
ripte_addr_t decrypt_aes(uint8_t out[16],  const uint8_t in[16],
			  const uint8_t *key, size_t key_size);
TEE_Result encrypt_aes(uint8_t out[16],  const uint8_t in[16],
			  const uint8_t *key, size_t key_size);
TEE_Result test_rsa(void);
TEE_Result test_ecc(void);

TEE_Result ta_ripte_init(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_ripte_event(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_ripte_quote(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_ripte_decrypt(uint32_t param_types, TEE_Param params[4]);
ftbl_entry_t *ftbl_bsearch( ftbl_entry_t *start, ftbl_entry_t *end, ripte_addr_t addr);

TEE_Result ta_ripte_decrypt_test(uint32_t param_types, TEE_Param params[4]);

#endif /* RIPTE_H */
