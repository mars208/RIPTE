#include <compiler.h>
#include <kernel/static_ta.h>
#include <tee_api_defines.h>
#include <trace.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tee/tee_cryp_provider.h>
#include <tee/tee_fs_key_manager.h>
#include <mm/core_memprot.h>
// #include <tomcrypt_hash.h>

#include <kernel/tee_time.h>

#include "ripte.h"

#define TA_NAME		"ripte.ta"

#define STA_RIPTE_UUID \
		{ 0x89f5fa4d, 0x163a, 0x498f, \
			{ 0x90, 0xd4, 0x60, 0xa8, 0xec, 0xdc, 0x2a, 0xeb} }

/* The Trusted Application Function ID(s) implemented in this TA */
#define TA_RIPTE_INIT		0
#define TA_RIPTE_EVENT	1
#define TA_RIPTE_QUOTE	2
#define TA_RIPTE_DECRYPT 	3

ripte_ctx_t* ctx;
static __inline void swap_bytes(uint8_t *a, uint8_t *b);

/*
 * Trusted Application Entry Points
 */

static TEE_Result create_ta(void)
{
	return TEE_SUCCESS;
}

static void destroy_ta(void)
{
}

static TEE_Result open_session(uint32_t ptype __unused,
			       TEE_Param params[TEE_NUM_PARAMS] __unused,
			       void **ppsess __unused)
{
	return TEE_SUCCESS;
}

static void close_session(void *psess __unused)
{
}


TEE_Result ta_ripte_init(uint32_t param_types, TEE_Param params[4])
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT,
						   TEE_PARAM_TYPE_VALUE_INPUT,
						   TEE_PARAM_TYPE_VALUE_INPUT,
						   TEE_PARAM_TYPE_VALUE_INPUT);
	ripte_addr_t main_start = params[0].value.a;
	ripte_addr_t main_end = params[0].value.b;

	btbl_entry_t *btbl_start = (btbl_entry_t *)params[1].value.a;
	btbl_entry_t *btbl_end = (btbl_entry_t *)btbl_start + (params[1].value.b - 1);
	ltbl_entry_t *ltbl_start = (ltbl_entry_t *)params[2].value.a;
	ltbl_entry_t *ltbl_end = (ltbl_entry_t *)ltbl_start + (params[2].value.b - 1);
	ftbl_entry_t *ftbl_start = (ftbl_entry_t *)params[3].value.a;
	ftbl_entry_t *ftbl_end = (ftbl_entry_t *)ftbl_start + (params[3].value.b - 1);
	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	ctx = (ripte_ctx_t*)malloc(sizeof(ripte_ctx_t));

	if (ripte_init(ctx, main_start, main_end, btbl_start, btbl_end, ltbl_start, ltbl_end, ftbl_start, ftbl_end) == 0)
	{
		return TEE_SUCCESS;
	}else{
		return TEE_ERROR_BAD_PARAMETERS;
	}
}


TEE_Result ta_ripte_event(uint32_t param_types, TEE_Param params[4])
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT,
						   TEE_PARAM_TYPE_VALUE_INPUT,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);
	ripte_event_t *event = (ripte_event_t *)malloc(sizeof(ripte_event_t));
	event->type = params[0].value.a;
	event->src_addr = params[0].value.b;
	event->dst_addr = params[1].value.a;
	event->lr_value = params[1].value.b;

	DMSG("%08x, %08x\n", event->src_addr,event->dst_addr );
	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;
	if (ripte_event(ctx, event) == 0)
	{
		return TEE_SUCCESS;
	}else{
		return TEE_ERROR_BAD_PARAMETERS;
	}

}


TEE_Result ta_ripte_quote(uint32_t param_types, TEE_Param params[4])
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT,
						   TEE_PARAM_TYPE_VALUE_INPUT,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);
	uint8_t *user_data = (uint8_t *)params[0].value.a;
	uint32_t user_data_len = params[0].value.b;
	uint8_t *out = (uint8_t *)params[1].value.a;
	uint32_t *out_len = (uint32_t *)params[1].value.b;

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	if (ripte_quote(ctx, user_data, user_data_len, out, out_len) == 0)
	{
		return TEE_SUCCESS;
	}else{
		return TEE_ERROR_BAD_PARAMETERS;
	}

	free(ctx);
}

TEE_Result test_rsa(void){
	struct rsa_keypair rsa_key;
	struct rsa_public_key pub_key;
	size_t key_size = 2048;
	uint8_t msg[] = "12345678";
	uint8_t sig[512];
	size_t sig_len;
	TEE_Result res;

	res = crypto_ops.acipher.alloc_rsa_keypair(&rsa_key, key_size);
	if (res != TEE_SUCCESS)
		DMSG("alloc key error");
	res = crypto_ops.acipher.alloc_rsa_public_key(&pub_key, key_size);
	if (res != TEE_SUCCESS)
		DMSG("alloc pub_key error");
	res = crypto_ops.acipher.gen_rsa_key(&rsa_key, key_size);
	if (res != TEE_SUCCESS)
		DMSG("gen error");
	pub_key.n = rsa_key.n;
	pub_key.e = rsa_key.e;

	res = crypto_ops.acipher.rsassa_sign(TEE_ALG_RSASSA_PKCS1_V1_5_SHA1, &rsa_key, -1, msg, sizeof(msg), sig, &sig_len);
	if (res != TEE_SUCCESS)
		DMSG("sign error");

	res = crypto_ops.acipher.rsassa_verify(TEE_ALG_RSASSA_PKCS1_V1_5_SHA1, &pub_key, -1,
				msg, sizeof(msg), sig, sig_len);

	crypto_ops.acipher.free_rsa_public_key(&pub_key);
	if (res != TEE_SUCCESS)
		DMSG("verify error");
	return TEE_SUCCESS;
}

TEE_Result test_ecc(void){
	return TEE_SUCCESS;
}

ripte_addr_t decrypt(ripte_addr_t lr_value, ripte_addr_t key, ripte_addr_t measurement){
	ripte_addr_t dec_addr;
	dec_addr = lr_value ^ key ^ measurement;
	return dec_addr;
}

TEE_Result encrypt_aes(uint8_t out[16],  const uint8_t in[16],
			  const uint8_t *key, size_t key_size){

	TEE_Result res;
	uint8_t *cipher_ctx = NULL;
	size_t ctx_size;
	uint32_t algo = TEE_ALG_AES_ECB_NOPAD;


	res = crypto_ops.cipher.get_ctx_size(algo, &ctx_size);
	if (res != TEE_SUCCESS)
		return res;

	cipher_ctx = malloc(ctx_size);
	if (!cipher_ctx)
		return TEE_ERROR_OUT_OF_MEMORY;

	res = crypto_ops.cipher.init(cipher_ctx, algo, TEE_MODE_ENCRYPT, key,
				     key_size, NULL, 0, NULL, 0);
	if (res != TEE_SUCCESS)
		goto out;

	res = crypto_ops.cipher.update(cipher_ctx, algo, TEE_MODE_ENCRYPT, true, in,
				       TEE_AES_BLOCK_SIZE, out);
	if (res != TEE_SUCCESS)
		goto out;

	crypto_ops.cipher.final(cipher_ctx, algo);
	res = TEE_SUCCESS;

out:
	free(cipher_ctx);
	return res;
}

TEE_Result decrypt_aes(uint8_t out[16],  const uint8_t in[16],
			  const uint8_t *key, size_t key_size){
	TEE_Result res;
	uint8_t *cipher_ctx = NULL;
	size_t ctx_size;
	uint32_t algo = TEE_ALG_AES_ECB_NOPAD;

	res = crypto_ops.cipher.get_ctx_size(algo, &ctx_size);
	if (res != TEE_SUCCESS)
		return res;

	cipher_ctx = malloc(ctx_size);
	if (!cipher_ctx)
		return TEE_ERROR_OUT_OF_MEMORY;

	res = crypto_ops.cipher.init(cipher_ctx, algo, TEE_MODE_DECRYPT, key,
				     key_size, NULL, 0, NULL, 0);
	// DMSG("%x", *lr_value);
	if (res != TEE_SUCCESS)
		goto out;

	res = crypto_ops.cipher.update(cipher_ctx, algo, TEE_MODE_DECRYPT, true, in,
				       TEE_AES_BLOCK_SIZE, out);

	if (res != TEE_SUCCESS)
		goto out;

	crypto_ops.cipher.final(cipher_ctx, algo);
	res = TEE_SUCCESS;

out:
	free(cipher_ctx);
	return res;
}

static __inline void
swap_bytes(uint8_t *a, uint8_t *b)
{
    uint8_t temp;

    temp = *a;
    *a = *b;
    *b = temp;
}

/*
 * Initialize an RC4 state buffer using the supplied key,
 * which can have arbitrary length.
 */
void
rc4_init(struct rc4_state *const state, const uint8_t *key, int keylen)
{
    uint8_t j;
    int i;

    /* Initialize state with identity permutation */
    for (i = 0; i < 256; i++)
        state->perm[i] = (uint8_t)i; 
    state->index1 = 0;
    state->index2 = 0;

    /* Randomize the permutation using key data */
    for (j = i = 0; i < 256; i++) {
        j += state->perm[i] + key[i % keylen]; 
        swap_bytes(&state->perm[i], &state->perm[j]);
    }
}

/*
 * Encrypt some data using the supplied RC4 state buffer.
 * The input and output buffers may be the same buffer.
 * Since RC4 is a stream cypher, this function is used
 * for both encryption and decryption.
 */
void
rc4_crypt(struct rc4_state *const state,
    const uint8_t *inbuf, uint8_t *outbuf, int buflen)
{
    int i;
    uint8_t j;

    for (i = 0; i < buflen; i++) {

        /* Update modification indicies */
        state->index1++;
        state->index2 += state->perm[state->index1];

        /* Modify permutation */
        swap_bytes(&state->perm[state->index1],
            &state->perm[state->index2]);

        /* Encrypt/decrypt next byte */
        j = state->perm[state->index1] + state->perm[state->index2];
        outbuf[i] = inbuf[i] ^ state->perm[j];
    }
}

ftbl_entry_t *ftbl_bsearch(ftbl_entry_t *start, ftbl_entry_t *end, ripte_addr_t addr)
{
	size_t i, high;
	ftbl_entry_t *e;
	uint8_t *lr_value;
	uint32_t lr_value_32;

	high = ((uint32_t)end - (uint32_t)start) / sizeof(ftbl_entry_t);

	e = start;
	for(i = 0; i < high + 1; i++){
		lr_value = e->lr;
		lr_value_32 = (lr_value[60] << 24) | (lr_value[61] << 16) | (lr_value[62] << 8) | lr_value[63];
		// DMSG("%x, %x, %x", (uint32_t)e, e->src, lr_value_32);
		if(lr_value_32 == addr){
			return e;
		}
		e = e + 1;
	}

	return NULL;
}

TEE_Result ta_ripte_decrypt(uint32_t param_types, TEE_Param params[4])
{

	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INOUT,
						   TEE_PARAM_TYPE_VALUE_INPUT,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);
	uint32_t dst_value = params[0].value.a;
	ftbl_entry_t *ftbl_ptr;
#ifdef TEST_ENC
	uint32_t end_addr = params[0].value.b;
	uint32_t end_addr_phys = params[1].value.a;
	uint32_t start_addr;

	uint32_t key;
	ripte_addr_t measurement;
	// ripte_hash_state_t state;

	uint32_t algo = TEE_ALG_SHA256;
	uint8_t *hash_ctx = NULL;
	size_t ctx_size;
	uint8_t digest[32];

	ripte_addr_t dst_addr = 0;
	int shift_num = 0;
	int ascii_to_num;
	
	uint8_t out1[64];
	uint8_t out2[64];
	uint8_t tmp_xor_result[32];
#endif

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	ftbl_ptr = ftbl_bsearch(ctx->ftbl_start, ctx->ftbl_end, dst_value);
#ifdef TEST_ENC
	start_addr = ftbl_ptr->dst;

	if (!core_mmu_add_mapping(MEM_AREA_NSEC_SHM, end_addr_phys-end_addr+start_addr, end_addr-start_addr))
		EMSG("Failed to map fdt");

	crypto_ops.hash.get_ctx_size(algo, &ctx_size);

	hash_ctx = malloc(ctx_size);
	crypto_ops.hash.init(hash_ctx, algo);
	crypto_ops.hash.update(hash_ctx, algo, (uint8_t *)phys_to_virt(end_addr_phys-end_addr+start_addr, MEM_AREA_NSEC_SHM), (end_addr-start_addr));

	crypto_ops.hash.final(hash_ctx, algo, digest, 32);
	free(hash_ctx);

	for (int i = 0; i < 64; ++i)
	{
		out1[i] = ftbl_ptr->lr[i];
	}

	for (int i = 0; i < 32; ++i)
	{
		tmp_xor_result[i] = 0;
	}

	for (int i = 63; i >= 0; --i)
	{
		if (out1[i] == '\0')
			continue;
		else{
			if (out1[i] >= '0' && out1[i] <= '9')
			{
				ascii_to_num = out1[i] - '0';
			}else if(out1[i] >= 'a' && out1[i] <= 'f'){
				ascii_to_num = out1[i] - 'a' + 10;
			}else{
				ascii_to_num = 0;
			}
			tmp_xor_result[i/2] += ascii_to_num<<shift_num;
			if(i % 2 == 0)
				shift_num = 0;
			else 
				shift_num = 4;
		}
	}

	dst_addr = (tmp_xor_result[28] << 24) | (tmp_xor_result[29] << 16) | (tmp_xor_result[30] << 8) | tmp_xor_result[31];
	key = 0x11223;
	measurement = (digest[28] << 24) | (digest[29] << 16) | (digest[30] << 8) | digest[31];

	params[0].value.a = decrypt(dst_addr, key, measurement);

	return TEE_SUCCESS;

	for (int i = 0; i < 32; ++i)
	{
		tmp_xor_result[i] = 0;
	}

	for (int i = 63; i >= 0; --i)
	{
		if (out2[i] == '\0')
			continue;
		else{
			if (out2[i] >= '0' && out2[i] <= '9')
			{
				ascii_to_num = out2[i] - '0';
			}else if(out2[i] >= 'a' && out2[i] <= 'f'){
				ascii_to_num = out2[i] - 'a' + 10;
			}else{
				ascii_to_num = 0;
			}
			tmp_xor_result[i/2] += ascii_to_num<<shift_num;
			if(i % 2 == 0)
				shift_num = 0;
			else 
				shift_num = 4;
		}
	}
	dst_addr = ((digest[28] ^ tmp_xor_result[28]) << 24) | ((digest[29] ^ tmp_xor_result[29]) << 16) | ((digest[30] ^ tmp_xor_result[30]) << 8) | (digest[31] ^ tmp_xor_result[31]);

	params[0].value.a = dst_addr;
#endif
#ifndef TEST_ENC 
	params[0].value.a = ftbl_ptr->src + 4;
#endif
	return TEE_SUCCESS;
}

/* Called when a command is invoked */
static TEE_Result invoke_command(void *psess __unused,
				 uint32_t cmd, uint32_t nParamTypes,
				 TEE_Param pParams[TEE_NUM_PARAMS])
{
	switch (cmd) {
	case TA_RIPTE_INIT:
		return ta_ripte_init(nParamTypes, pParams);

	case TA_RIPTE_EVENT:
		return ta_ripte_event(nParamTypes, pParams);

	case TA_RIPTE_QUOTE:
		return ta_ripte_quote(nParamTypes, pParams);

	case TA_RIPTE_DECRYPT:
		return ta_ripte_decrypt(nParamTypes, pParams);

	default:
		return TEE_ERROR_BAD_PARAMETERS;
	}
}

static_ta_register(.uuid = STA_RIPTE_UUID, .name = TA_NAME,
		   .create_entry_point = create_ta,
		   .destroy_entry_point = destroy_ta,
		   .open_session_entry_point = open_session,
		   .close_session_entry_point = close_session,
		   .invoke_command_entry_point = invoke_command);