#include <err.h>
#include "ripte_stub.h"
#include "ripte_helpers.h"
#include "ripte_ta.h"
#include "btbl.h"
#include "ltbl.h"

uint32_t ripte_init(const ripte_addr_t main_start, const ripte_addr_t main_end)
{
	TEEC_Operation op;
	TEEC_UUID uuid = TA_ripte_UUID;

	res = ripte_teec_ctx_init();
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

	res = ripte_teec_open_session(&session, &uuid, NULL, &ret_orig);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, ret_orig);

	memset(&op, 0, sizeof(op));
	
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_VALUE_INPUT,
					 TEEC_VALUE_INPUT, TEEC_VALUE_INPUT);
	op.params[0].value.a = (uint32_t)main_start;
	op.params[0].value.b = (uint32_t)main_end;
	op.params[1].value.a = get_physical_address(btbl);
	op.params[1].value.b = len_btbl;
	op.params[2].value.a = get_physical_address(ltbl);
	op.params[2].value.b = len_ltbl ;
	op.params[3].value.a = get_physical_address(ftbl);
	op.params[3].value.b = len_ftbl ;

	res = TEEC_InvokeCommand(&session, TA_ripte_INIT, &op, &ret_orig);

	return res;
}

uint32_t ripte_quote(const uint8_t *user_data, const uint32_t user_data_len,
		   uint8_t *out, uint32_t *out_len)
{
	TEEC_Operation op;
	uint32_t ret_orig;


	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_VALUE_INPUT,
					 TEEC_NONE, TEEC_NONE);

	op.params[0].value.a = (uint32_t)user_data;
	op.params[0].value.b = (uint32_t)user_data_len;
	op.params[1].value.a = (uint32_t)out;
	op.params[1].value.b = (uint32_t)out_len;

	res = TEEC_InvokeCommand(&session, TA_ripte_QUOTE, &op, &ret_orig);

	TEEC_CloseSession(&session);

	ripte_teec_ctx_deinit();

	return res;
}

