#ifndef RIPTE_STUB_H
#define RIPTE_STUB_H

#ifdef RIPTE_H
#error "RIPTE_H already defined, did you mean to call the secure world API?"
#endif

#include <stddef.h>
#include <stdint.h>

#include "btbl.h"
#include "ltbl.h"

#include "ripte_common.h"

/* Normal world API */

/*!
 * \brief ripte_init
 * Initialize the RIPTE subsystem.
 * \param main_start Start address of main function
 * \param main_end End address of main function
 * \param u_start Start address of user code to be attested
 * \param u_end End address of user code to be attested
 */
uint32_t ripte_init(const ripte_addr_t main_start, const ripte_addr_t main_end);/*,
		  const btbl_entry_t *btbl_start, const btbl_entry_t *btbl_end,
		  const ltbl_entry_t *ltbl_start, const ltbl_entry_t *ltbl_end);*/

/*!
 * \brief ripte_quote
 * Quote the current digest value.
 * \param out Output buffer for storing the digest quote
 * \param outlen Length of the output buffer for storing the digest quote
 */
uint32_t ripte_quote(const uint8_t *user_data, const uint32_t user_data_len,
		   uint8_t *out, uint32_t *out_len);

#endif /* RIPTE_STUB_H */

