#include "ripte_common.h"
#include "ripte_ta.h"
#include "ripte_helpers.h"
#include "trampoline.h"
#include "btbl.h"
#include "ltbl.h"
#include "stdio.h"
#include "stdlib.h"

#define INVALID_LR_VALUE 0
// #define TEST_TIME
/* just for test */
int hook_ret0(void){
	return 0;
}

/* search branch table for which address stores the target */
ripte_addr_t btbl_bsearch(ripte_addr_t target){
	btbl_entry_t btbl_object;
	int ismall = 0;
	int ilarge = len_btbl;
	int i;
	/*for (int i = 0; i < len_btbl; ++i)
	{
		btbl_object = btbl[i];
		if (btbl_object.src == target)
		{
			return btbl_object.dst;
		}
	}*/
	while(1){
		i = (ismall + ilarge)/2; 
		btbl_object = btbl[i];
		if (btbl_object.src == target)
		{
			return btbl_object.dst;
		}else if(btbl_object.src < target){
			ismall = i + 1;
		}else{
			ilarge = i;
		}
	}
	return 0;
}

/* search function table for which address stores the target */
ripte_addr_t ftbl_bsearch_lr(ripte_addr_t target){
	ftbl_entry_t ftbl_object;
	int ismall = 0;
	int ilarge = len_ftbl;
	int i;
	uint8_t *lr_value;
	uint32_t lr_value_32;

	/*for (int i = 0; i < len_ftbl; ++i)
	{
		ftbl_object = ftbl[i];
		if (ftbl_object.src == target)
		{
			uint8_t *lr_value = ftbl_object.lr;
			// uint32_t lr_value_32 = lr_value[0] | (lr_value[1] << 8) | (lr_value[2] << 16) | (lr_value[3] << 24);
			uint32_t lr_value_32 = (lr_value[60] << 24) | (lr_value[61] << 16) | (lr_value[62] << 8) | lr_value[63];
			return lr_value_32;
		}
	}*/
	while(1){
		i = (ismall + ilarge)/2; 
		ftbl_object = ftbl[i];
		if (ftbl_object.src == target)
		{
			lr_value = ftbl_object.lr;
			// uint32_t lr_value_32 = lr_value[0] | (lr_value[1] << 8) | (lr_value[2] << 16) | (lr_value[3] << 24);
			lr_value_32 = (lr_value[60] << 24) | (lr_value[61] << 16) | (lr_value[62] << 8) | lr_value[63];
			return lr_value_32;
		}else if(ftbl_object.src < target){
			ismall = i + 1;
		}else{
			ilarge = i;
		}
	}
	return 0;
}

uint8_t ftbl_bsearch_type(ripte_addr_t target){
	ftbl_entry_t ftbl_object;
	int ismall = 0;
	int ilarge = len_ftbl;
	int i;

	/*for (int i = 0; i < len_ftbl; ++i)
	{
		ftbl_object = ftbl[i];
		if (ftbl_object.src == target - 4)
		{
			return ftbl_object.type;
		}
	}*/
	while(1){
		i = (ismall + ilarge)/2; 
		ftbl_object = ftbl[i];
		if (ftbl_object.src == target - 4)
		{
			return ftbl_object.type;
		}else if(ftbl_object.src < target){
			ismall = i + 1;
		}else{
			ilarge = i;
		}
	}
	return 0;
}

uint8_t fdtbl_bsearch_type(ripte_addr_t target){
	fdtbl_entry_t fdtbl_object;
	// int ismall = 0;
	// int ilarge = len_fdtbl;
	int i;

	for (int i = 0; i < len_fdtbl; ++i)
	{
		fdtbl_object = fdtbl[i];
		if (fdtbl_object.end == target)
		{
			return fdtbl_object.type;
		}
	}
	/*while(1){
		i = (ismall + ilarge)/2; 
		fdtbl_object = fdtbl[i];
		if (fdtbl_object.end == target)
		{
			return fdtbl_object.type;
		}else if(fdtbl_object.end < target){
			ismall = i + 1;
		}else{
			ilarge = i;
		}
	}*/
	return 0;
}

ftbl_entry_t *ftbl_bsearch(ripte_addr_t addr)
{
	size_t i, high;
	ftbl_entry_t *e;
	uint8_t *lr_value;
	uint32_t lr_value_32;

	high = len_ftbl;

	e = ftbl;
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

void hook_invoke_ta(TEEC_Operation op){
	// TEEC_Result res;
	// TEEC_Session session = { 0 };
	// TEEC_UUID uuid = TA_ripte_UUID;
	// uint32_t ret_orig;
	//ripte_teec_open_session(&session, &uuid, NULL, &ret_orig);

	res = TEEC_InvokeCommand(&session, TA_ripte_EVENT, &op, &ret_orig);

	//ctx = op.params[0].value.a;
	//TEEC_CloseSession(&session);
}

void set_op_params(TEEC_Operation *op, uint32_t type, ripte_addr_t src_addr, ripte_addr_t dst_addr, ripte_addr_t lr_value){
	/*
	 * Prepare the argument. Pass a value in the first parameter,
	 * the remaining three parameters are unused.
	 */
	/* Clear the TEEC_Operation struct */
	memset(op, 0, sizeof(op));
	op->paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_VALUE_INPUT,
					 TEEC_NONE, TEEC_NONE);
	op->params[0].value.a = type;
	op->params[0].value.b = get_physical_address(src_addr);
	op->params[1].value.a = get_physical_address(dst_addr);
	op->params[1].value.b = lr_value;

}

ripte_addr_t ta_decrypt_test(ripte_addr_t lr_value, ripte_addr_t src_addr){
	ftbl_entry_t *ftbl_ptr;
	ftbl_ptr = ftbl_bsearch(lr_value);
	return ftbl_ptr->src + 4;
}

ripte_addr_t ta_decrypt(ripte_addr_t lr_value, ripte_addr_t src_addr){
	TEEC_Operation op;
	#ifdef TEST_TIME
	struct  timeval  start;
	struct  timeval  end;
	unsigned long timer;
	#endif 
	#ifdef TEST_TIME
	gettimeofday(&start,NULL);
	#endif
	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_VALUE_INPUT,
					 TEEC_NONE, TEEC_NONE);
	op.params[0].value.a = lr_value;
	op.params[0].value.b = src_addr;
	op.params[1].value.a = get_physical_address((void*)src_addr);
	TEEC_InvokeCommand(&session, TA_ripte_DECRYPT, &op, &ret_orig);
	#ifdef TEST_TIME
	gettimeofday(&end,NULL);
	timer = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
 	printf("TEEC_InvokeCommand timer = %ld us\n",timer);
	#endif
	return op.params[0].value.a ;
}

ripte_addr_t decrypt(ripte_addr_t lr_value, ripte_addr_t key, ripte_addr_t measurement){
	ripte_addr_t dec_addr;
	dec_addr = lr_value ^ key ^ measurement;
	return dec_addr;
}

/* hook code for b command */
void hook_b(void){
	uint32_t type;
	ripte_addr_t src_addr,dst_addr,lr_value;
	
	type = ripte_EVENT_B;
	__asm__ __volatile__ ( 
			"sub %0, lr, #4"
			: "=r"(src_addr)
			);
	dst_addr = btbl_bsearch(src_addr);
	if(dst_addr == 0) {
		type = ripte_EVENT_ERROR;
	}
	lr_value = INVALID_LR_VALUE;
	b_count++;
	// TEEC_Operation op;
	// set_op_params(&op, type, src_addr, dst_addr, lr_value);
	// hook_invoke_ta(op);
	//printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	__asm__ __volatile__ (
			"mov lr, %1\n\t" 
			// "adds r7, #0x50\n\t"
			// "mov sp, r7\n\t"
			// "pop {r4, r5, r7}\n\t"
			// "pop {r0}\n\t"
			"sub sp, r11, #4\n\t"
			"ldmfd sp!, {r11, r12}\n\t"
			"bx %0"
			:
			: "r"(dst_addr), "r"(lr_value)
			);
}

void hook_bl(void){
	uint32_t type;
	ripte_addr_t src_addr,dst_addr,lr_value;
	ripte_addr_t r0,r1,r2,r3;
	double d0, d1, d2, d3;
	#ifdef TEST_TIME
	struct  timeval  start;
	struct  timeval  end, end1, end2;
	unsigned long timer;
	#endif 
	__asm__ __volatile__ ( 
			"str r0, %0"
			: "=m"(r0)
			);
	__asm__ __volatile__ ( 
			"str r1, %0"
			: "=m"(r1)
			);
	__asm__ __volatile__ ( 
			"str r2, %0"
			: "=m"(r2)
			);
	__asm__ __volatile__ ( 
			"str r3, %0"
			: "=m"(r3)
			);
	__asm__ __volatile__ ( 
			"vstr d0, %0"
			: "=m"(d0)
			);
	__asm__ __volatile__ ( 
			"vstr d1, %0"
			: "=m"(d1)
			);
	__asm__ __volatile__ ( 
			"vstr d2, %0"
			: "=m"(d2)
			);
	__asm__ __volatile__ ( 
			"vstr d3, %0"
			: "=m"(d3)
			);

	type = ripte_EVENT_BL;
	__asm__ __volatile__ ( 
			"sub %0, lr, #4\n\t"
			// "mov %1, lr"
			: "=r"(src_addr)//, "=r"(lr_value)
			);
	#ifdef TEST_TIME
	gettimeofday(&start,NULL);
	#endif
	dst_addr = btbl_bsearch(src_addr);
	#ifdef TEST_TIME
	gettimeofday(&end1,NULL);
	#endif
	lr_value = ftbl_bsearch_lr(src_addr);
	#ifdef TEST_TIME
	gettimeofday(&end2,NULL);
	#endif
	if(dst_addr == 0) {
		type = ripte_EVENT_ERROR;
	}
	list_push(program_stack, (void*)lr_value);
	bl_count++;

	#ifdef TEST_TIME
	gettimeofday(&end,NULL);
	timer = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
 	printf("hook_bl timer = %ld us\n",timer);
	timer = 1000000 * (end1.tv_sec-start.tv_sec)+ end1.tv_usec-start.tv_usec;
 	printf("btbl_bsearch timer = %ld us\n",timer);
	timer = 1000000 * (end2.tv_sec-end1.tv_sec)+ end2.tv_usec-end1.tv_usec;
 	printf("ftbl_bsearch_lr timer = %ld us\n",timer);
	timer = 1000000 * (end.tv_sec-end2.tv_sec)+ end.tv_usec-end2.tv_usec;
 	printf("list_push timer = %ld us\n",timer);
	#endif

	// TEEC_Operation op;
	// set_op_params(&op, type, src_addr, dst_addr, lr_value);
	// hook_invoke_ta(op);

	__asm__ __volatile__ ( 
			"ldr r0, %0"
			:
			: "m"(r0)
			);
	__asm__ __volatile__ ( 
			"ldr r1, %0"
			:
			: "m"(r1)
			);
	__asm__ __volatile__ ( 
			"ldr r2, %0"
			:
			: "m"(r2)
			);
	__asm__ __volatile__ ( 
			"ldr lr, %0"
			:
			: "m"(lr_value)
			);
	__asm__ __volatile__ ( 
			"vldr d0, %0"
			:
			: "m"(d0)
			);
	__asm__ __volatile__ ( 
			"vldr d1, %0"
			:
			: "m"(d1)
			);
	__asm__ __volatile__ ( 
			"vldr d2, %0"
			:
			: "m"(d2)
			);
	__asm__ __volatile__ ( 
			"vldr d3, %0"
			:
			: "m"(d3)
			);
	__asm__ __volatile__ ( 
			// "adds r7, #0x50\n\t"
			// "mov sp, r7\n\t"
			// "pop {r4, r5, r7}\n\t"
			// "pop {r0}\n\t"
			// "pop {r0, r1, r2}\n\t"
			"ldr r12, %0\n\t"
			"ldr r3, %1\n\t"
			"sub sp, r11, #4\n\t"
			// "str %0, [r0, #8]\n\t"
			"ldmfd sp!, {r11}\n\t"
			"add sp, sp, #4\n\t"
			"bx r12"
			:
			: "m"(dst_addr), "m"(r3)
			);
}

void hook_bx_lr(void){
	uint32_t type;
	ripte_addr_t src_addr,dst_addr,lr_value;
	ripte_addr_t r0, r4, r5, r6, r7, r8, r9, r10;
	#ifdef TEST_TIME
	struct  timeval  start;
	struct  timeval  end;
	unsigned long timer;
	#endif 
	__asm__ __volatile__ ( 
			"str r0, %0"
			: "=m"(r0)
			);
	__asm__ __volatile__ ( 
			"str r4, %0"
			: "=m"(r4)
			);
	__asm__ __volatile__ ( 
			"str r5, %0"
			: "=m"(r5)
			);
	__asm__ __volatile__ ( 
			"str r6, %0"
			: "=m"(r6)
			);
	__asm__ __volatile__ ( 
			"str r7, %0"
			: "=m"(r7)
			);
	__asm__ __volatile__ ( 
			"str r8, %0"
			: "=m"(r8)
			);
	__asm__ __volatile__ ( 
			"str r9, %0"
			: "=m"(r9)
			);
	__asm__ __volatile__ ( 
			"str r10, %0"
			: "=m"(r10)
			);

	type = ripte_EVENT_BX_LR;
	__asm__ __volatile__ ( 
			"sub %0, lr, #4"
			: "=r"(src_addr)
			);

	#ifdef TEST_TIME
	gettimeofday(&start,NULL);
	#endif
	dst_addr = list_pop(program_stack);
	lr_value = dst_addr;

	// ripte_addr_t key = 0x11223;
	// ripte_addr_t measurement = 0;
	// lr_value = decrypt(lr_value, key, measurement);
	if ((dst_addr & 0xFFF00000) != 0)
	{
		// lr_value = ta_decrypt(dst_addr, src_addr);
		lr_value = ta_decrypt_test(dst_addr, src_addr);
	}else{
		lr_value = dst_addr;
	}
	// TEEC_Operation op;
	// set_op_params(&op, type, src_addr, dst_addr, lr_value);
	// hook_invoke_ta(op);

	#ifdef TEST_TIME
	gettimeofday(&end,NULL);
	timer = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
 	printf("hook_bx_lr timer = %ld us\n",timer);
	#endif
	__asm__ __volatile__ ( 
			"mov r0, %0"
			:
			: "r"(r0)
			);
	__asm__ __volatile__ ( 
			"ldr r4, %0"
			:
			: "m"(r4)
			);
	__asm__ __volatile__ ( 
			"ldr r5, %0"
			:
			: "m"(r5)
			);
	__asm__ __volatile__ ( 
			"ldr r6, %0"
			:
			: "m"(r6)
			);
	__asm__ __volatile__ ( 
			"ldr r7, %0"
			:
			: "m"(r7)
			);
	__asm__ __volatile__ ( 
			"ldr r8, %0"
			:
			: "m"(r8)
			);
	__asm__ __volatile__ ( 
			"ldr r9, %0"
			:
			: "m"(r9)
			);
	__asm__ __volatile__ ( 
			"ldr r10, %0"
			:
			: "m"(r10)
			);
	__asm__ __volatile__ ( 
			"mov lr, %0\n\t" 
			// "adds r7, #0x50\n\t"
			// "mov sp, r7\n\t"
			// "pop {r4, r5, r7}\n\t"
			// "pop {r0}\n\t"
			"sub sp, r11, #4\n\t"
			"ldmfd sp!, {r11, r12}\n\t"
			"bx %0"
			:
			: "r"(lr_value)
			);
}

void hook_pop_fp_pc(void){
	// uint32_t type;
	ripte_addr_t src_addr,dst_addr,lr_value;
	ripte_addr_t r0,r4, r5, r6, r7, r8, r9, r10;
	uint8_t return_type;
	#ifdef TEST_TIME
	struct  timeval  start;
	struct  timeval  end, end1, end2;
	unsigned long timer;
	#endif 
	__asm__ __volatile__ ( 
			"str r0, %0"
			: "=m"(r0)
			);
	__asm__ __volatile__ ( 
			"str r4, %0"
			: "=m"(r4)
			);
	__asm__ __volatile__ ( 
			"str r5, %0"
			: "=m"(r5)
			);
	__asm__ __volatile__ ( 
			"str r6, %0"
			: "=m"(r6)
			);
	__asm__ __volatile__ ( 
			"str r7, %0"
			: "=m"(r7)
			);
	__asm__ __volatile__ ( 
			"str r8, %0"
			: "=m"(r8)
			);
	__asm__ __volatile__ ( 
			"str r9, %0"
			: "=m"(r9)
			);
	__asm__ __volatile__ ( 
			"str r10, %0"
			: "=m"(r10)
			);

	// type = ripte_EVENT_POP_FP_PC;
	__asm__ __volatile__ ( 
			"sub %0, lr, #4"
			: "=r"(src_addr)
			);
	#ifdef TEST_TIME
	gettimeofday(&start,NULL);
	#endif
	lr_value = INVALID_LR_VALUE;
	dst_addr = list_pop(program_stack);
	#ifdef TEST_TIME
	gettimeofday(&end1,NULL);
	#endif

	if (dst_addr == NULL)
	{
		// TEEC_Operation op;
		// set_op_params(&op, type, src_addr, dst_addr, lr_value);
		// hook_invoke_ta(op);

		#ifdef TEST_TIME
		gettimeofday(&end,NULL);
		timer = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
	 	printf("hook_pop_fp_pc timer = %ld us\n",timer);
		#endif
		__asm__ __volatile__ ( 
			"mov r0, %0"
			:
			: "r"(r0)
			);
		__asm__ __volatile__ ( 
			// "adds r7, #0x50\n\t"
			// "mov sp, r7\n\t"
			// "pop {r4, r5, r7}\n\t"
			// "pop {r0}\n\t"
			"sub sp, r11, #4\n\t"
			"pop {r1, r2}\n\t"
			"pop {r11, pc}"
			);
	}

	// ripte_addr_t key = 0x11223;
	// ripte_addr_t measurement = 0;
	// dst_addr = decrypt(dst_addr, key, measurement);
	if ((dst_addr & 0xFFF00000) != 0)
	{
		// dst_addr = ta_decrypt(dst_addr, src_addr);
		dst_addr = ta_decrypt_test(dst_addr, src_addr);
		#ifdef TEST_TIME
		gettimeofday(&end2,NULL);
		#endif
		return_type = ftbl_bsearch_type(dst_addr);
	}else{
		return_type = fdtbl_bsearch_type(src_addr);
	}
	
	#ifdef TEST_TIME
	gettimeofday(&end,NULL);
	timer = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
 	printf("hook_pop_fp_pc timer = %ld us\n",timer);
	timer = 1000000 * (end1.tv_sec-start.tv_sec)+ end1.tv_usec-start.tv_usec;
 	printf("list_pop timer = %ld us\n",timer);
	timer = 1000000 * (end2.tv_sec-end1.tv_sec)+ end2.tv_usec-end1.tv_usec;
 	printf("ta_decrypt timer = %ld us\n",timer);
	timer = 1000000 * (end.tv_sec-end2.tv_sec)+ end.tv_usec-end2.tv_usec;
 	printf("ftbl_bsearch_type timer = %ld us\n",timer);
	#endif
	// TEEC_Operation op;
	// set_op_params(&op, type, src_addr, dst_addr, lr_value);
	// hook_invoke_ta(op);

	__asm__ __volatile__ ( 
			"ldr r4, %0"
			:
			: "m"(r4)
			);
	__asm__ __volatile__ ( 
			"ldr r5, %0"
			:
			: "m"(r5)
			);
	__asm__ __volatile__ ( 
			"ldr r6, %0"
			:
			: "m"(r6)
			);
	__asm__ __volatile__ ( 
			"ldr r7, %0"
			:
			: "m"(r7)
			);
	__asm__ __volatile__ ( 
			"ldr r8, %0"
			:
			: "m"(r8)
			);
	__asm__ __volatile__ ( 
			"ldr r9, %0"
			:
			: "m"(r9)
			);
	__asm__ __volatile__ ( 
			"ldr r10, %0"
			:
			: "m"(r10)
			);
	if (return_type == 0)
	{
		__asm__ __volatile__ ( 
				"mov r0, %0"
				:
				: "r"(r0)
				);
		__asm__ __volatile__ ( 
				"sub sp, r11, #4\n\t"
				"pop {r1, r2}\n\t"
				"pop {r11, r12}\n\t"
				"bx %0"
				:
				: "r"(dst_addr)
				);
	}else if(return_type == 1)
	{
		__asm__ __volatile__ ( 
				"mov r0, %0"
				:
				: "r"(r0)
				);
		__asm__ __volatile__ ( 
				"sub sp, r11, #4\n\t"
				"pop {r1, r2}\n\t"
				"pop {r4, r11, r12}\n\t"
				"bx %0"
				:
				: "r"(dst_addr)
				);
	}else if(return_type == 2)
	{
		__asm__ __volatile__ ( 
				"mov r0, %0"
				:
				: "r"(r0)
				);
		__asm__ __volatile__ ( 
				"sub sp, r11, #4\n\t"
				"pop {r1, r2}\n\t"
				"pop {r4, r5, r11, r12}\n\t"
				"bx %0"
				:
				: "r"(dst_addr)
				);
	}else if(return_type == 3)
	{
		__asm__ __volatile__ ( 
				"mov r0, %0"
				:
				: "r"(r0)
				);
		__asm__ __volatile__ ( 
				"sub sp, r11, #4\n\t"
				"pop {r1, r2}\n\t"
				"pop {r4, r5, r6, r11, r12}\n\t"
				"bx %0"
				:
				: "r"(dst_addr)
				);
	}else if(return_type == 4)
	{
		__asm__ __volatile__ ( 
				"mov r0, %0"
				:
				: "r"(r0)
				);
		__asm__ __volatile__ ( 
				"sub sp, r11, #4\n\t"
				"pop {r1, r2}\n\t"
				"pop {r4, r5, r6, r7, r11, r12}\n\t"
				"bx %0"
				:
				: "r"(dst_addr)
				);
	}else if(return_type == 5)
	{
		__asm__ __volatile__ ( 
				"mov r0, %0"
				:
				: "r"(r0)
				);
		__asm__ __volatile__ ( 
				"sub sp, r11, #4\n\t"
				"pop {r1, r2}\n\t"
				"pop {r4, r5, r6, r7, r8, r11, r12}\n\t"
				"bx %0"
				:
				: "r"(dst_addr)
				);
	}else if(return_type == 6)
	{
		__asm__ __volatile__ ( 
				"mov r0, %0"
				:
				: "r"(r0)
				);
		__asm__ __volatile__ ( 
				"sub sp, r11, #4\n\t"
				"pop {r1, r2}\n\t"
				"pop {r4, r5, r6, r7, r8, r9, r11, r12}\n\t"
				"bx %0"
				:
				: "r"(dst_addr)
				);
	}else if(return_type == 7)
	{
		__asm__ __volatile__ ( 
				"mov r0, %0"
				:
				: "r"(r0)
				);
		__asm__ __volatile__ ( 
				"sub sp, r11, #4\n\t"
				"pop {r1, r2}\n\t"
				"pop {r4, r5, r6, r7, r8, r9, r10, r11, r12}\n\t"
				"bx %0"
				:
				: "r"(dst_addr)
				);
	}else if(return_type == 8)
	{
		__asm__ __volatile__ ( 
				"mov r0, %0"
				:
				: "r"(r0)
				);
		__asm__ __volatile__ ( 
				"ldr r1, %0\n\t"
				"sub sp, r11, #4\n\t"
				"pop {r2, r3}\n\t"
				"pop {r3, r4, r5, r6, r7, r8, r9, r10, r11, r12}\n\t"
				"bx r1"
				:
				: "m"(dst_addr)
				);
	}else if(return_type == 9)
	{
		__asm__ __volatile__ ( 
				"mov r0, %0"
				:
				: "r"(r0)
				);
		__asm__ __volatile__ ( 
				"sub sp, r11, #4\n\t"
				"pop {r11, r12}\n\t"
				"pop {r4, r12}\n\t"
				"bx %0"
				:
				: "r"(dst_addr)
				);
	}else if(return_type == 10)
	{
		__asm__ __volatile__ ( 
				"mov r0, %0"
				:
				: "r"(r0)
				);
		__asm__ __volatile__ ( 
				"sub sp, r11, #4\n\t"
				"pop {r11, r12}\n\t"
				"pop {r4, r5, r12}\n\t"
				"bx %0"
				:
				: "r"(dst_addr)
				);
	}else if(return_type == 11)
	{
		__asm__ __volatile__ ( 
				"mov r0, %0"
				:
				: "r"(r0)
				);
		__asm__ __volatile__ ( 
				"sub sp, r11, #4\n\t"
				"pop {r11, r12}\n\t"
				"pop {r4, r5, r6, r12}\n\t"
				"bx %0"
				:
				: "r"(dst_addr)
				);
	}else if(return_type == 12)
	{
		__asm__ __volatile__ ( 
				"mov r0, %0"
				:
				: "r"(r0)
				);
		__asm__ __volatile__ ( 
				"sub sp, r11, #4\n\t"
				"pop {r11, r12}\n\t"
				"pop {r4, r5, r6, r7, r12}\n\t"
				"bx %0"
				:
				: "r"(dst_addr)
				);
	}else if(return_type == 13)
	{
		__asm__ __volatile__ ( 
				"mov r0, %0"
				:
				: "r"(r0)
				);
		__asm__ __volatile__ ( 
				"sub sp, r11, #4\n\t"
				"pop {r11, r12}\n\t"
				"pop {r4, r5, r6, r7, r8, r12}\n\t"
				"bx %0"
				:
				: "r"(dst_addr)
				);
	}else if(return_type == 14)
	{
		__asm__ __volatile__ ( 
				"mov r0, %0"
				:
				: "r"(r0)
				);
		__asm__ __volatile__ ( 
				"sub sp, r11, #4\n\t"
				"pop {r11, r12}\n\t"
				"pop {r4, r5, r6, r7, r8, r9, r12}\n\t"
				"bx %0"
				:
				: "r"(dst_addr)
				);
	}else if(return_type == 15)
	{
		__asm__ __volatile__ ( 
				"mov r0, %0"
				:
				: "r"(r0)
				);
		__asm__ __volatile__ ( 
				"sub sp, r11, #4\n\t"
				"pop {r11, r12}\n\t"
				"pop {r4, r5, r6, r7, r8, r9, r10, r12}\n\t"
				"bx %0"
				:
				: "r"(dst_addr)
				);
	}else if(return_type == 16)
	{
		__asm__ __volatile__ ( 
				"mov r0, %0"
				:
				: "r"(r0)
				);
		__asm__ __volatile__ ( 
				"ldr r1, %0\n\t"
				"sub sp, r11, #4\n\t"
				"pop {r11, r12}\n\t"
				"pop {r3, r4, r5, r6, r7, r8, r9, r10, r12}\n\t"
				"bx r1"
				:
				: "m"(dst_addr)
				);
	}else if(return_type == 17)
	{
		__asm__ __volatile__ ( 
				"mov r0, %0"
				:
				: "r"(r0)
				);
		__asm__ __volatile__ ( 
				"ldr r1, %0\n\t"
				"sub sp, r11, #4\n\t"
				"pop {r11, r12}\n\t"
				"pop {r12}\n\t"
				"bx r1"
				:
				: "m"(dst_addr)
				);
	}
}

void hook_pop_fp_lr(void){
	// uint32_t type;
	// ripte_addr_t src_addr,dst_addr;
	ripte_addr_t lr_value;
	ripte_addr_t r12_value,temp;

	// type = ripte_EVENT_POP_FP_LR;
	__asm__ __volatile__ ( 
			"pop {fp, %0}\n\t"
			"mov %1, lr"
			: "=r"(r12_value), "=r"(lr_value)
			);
	temp = r12_value;
	r12_value = lr_value;
	lr_value = temp;

	// list_push(program_stack, lr_value);

	__asm__ __volatile__ (
			// "adds r7, #0x14\n\t"
			// "mov sp, r7\n\t"
			// "ldr r7, [sp], #4\n\t" 
			"bx %0"
			: 
			: "r"(r12_value)
			);
}

void hook_blx(void){
	// uint32_t type;
	ripte_addr_t src_addr,lr_value;

	ripte_addr_t r0,r1,r2,r3,r12;
	double d0, d1, d2, d3;
	uint8_t reg_type; 

	__asm__ __volatile__ ( 
			"str r0, %0"
			: "=m"(r0)
			);
	__asm__ __volatile__ ( 
			"str r1, %0"
			: "=m"(r1)
			);
	__asm__ __volatile__ ( 
			"str r2, %0"
			: "=m"(r2)
			);
	__asm__ __volatile__ ( 
			"str r3, %0"
			: "=m"(r3)
			);
	__asm__ __volatile__ ( 
			"str r12, %0"
			: "=m"(r12)
			);
	__asm__ __volatile__ ( 
			"vstr d0, %0"
			: "=m"(d0)
			);
	__asm__ __volatile__ ( 
			"vstr d1, %0"
			: "=m"(d1)
			);
	__asm__ __volatile__ ( 
			"vstr d2, %0"
			: "=m"(d2)
			);
	__asm__ __volatile__ ( 
			"vstr d3, %0"
			: "=m"(d3)
			);

	// type = ripte_EVENT_BLX_R3;
	__asm__ __volatile__ ( 
			"sub %0, lr, #4\n\t"
			// "mov %1, r3\n\t"
			"mov %1, lr"
			: "=r"(src_addr),  "=r"(lr_value)
			);


	list_push(program_stack, (void*)lr_value);
	reg_type = ftbl_bsearch_type(lr_value);
	blx_count++;
	// TEEC_Operation op;
	// set_op_params(&op, type, src_addr, dst_addr, lr_value);
	// hook_invoke_ta(op);

	__asm__ __volatile__ ( 
			"ldr r0, %0"
			:
			: "m"(r0)
			);
	__asm__ __volatile__ ( 
			"ldr r1, %0"
			:
			: "m"(r1)
			);
	__asm__ __volatile__ ( 
			"ldr r2, %0"
			:
			: "m"(r2)
			);
	__asm__ __volatile__ ( 
			"ldr lr, %0"
			:
			: "m"(lr_value)
			);
	__asm__ __volatile__ ( 
			"vldr d0, %0"
			:
			: "m"(d0)
			);
	__asm__ __volatile__ ( 
			"vldr d1, %0"
			:
			: "m"(d1)
			);
	__asm__ __volatile__ ( 
			"vldr d2, %0"
			:
			: "m"(d2)
			);
	__asm__ __volatile__ ( 
			"vldr d3, %0"
			:
			: "m"(d3)
			);

	if (reg_type == 0)
	{
		__asm__ __volatile__ ( 
			"sub sp, r11, #4\n\t"
			"ldmfd sp!, {r11, r12}\n\t"
			"bx %0"
			:
			: "r"(r3)
			);
	}else if(reg_type == 1)
	{
		__asm__ __volatile__ ( 
			"ldr r3, %0\n\t"
			"sub sp, r11, #4\n\t"
			"ldmfd sp!, {r11, r12}\n\t"
			"bx r4"
			:
			: "m"(r3)
			);
	}else if(reg_type == 2)
	{
		__asm__ __volatile__ ( 
			"ldr r3, %0\n\t"
			"sub sp, r11, #4\n\t"
			"ldmfd sp!, {r11, r12}\n\t"
			"bx r5"
			:
			: "m"(r3)
			);
	}else if(reg_type == 3)
	{
		__asm__ __volatile__ ( 
			"ldr r3, %0\n\t"
			"sub sp, r11, #4\n\t"
			"ldmfd sp!, {r11, r12}\n\t"
			"bx r6"
			:
			: "m"(r3)
			);
	}else if(reg_type == 4)
	{
		__asm__ __volatile__ ( 
			"ldr r3, %0\n\t"
			"sub sp, r11, #4\n\t"
			"ldmfd sp!, {r11, r12}\n\t"
			"bx r7"
			:
			: "m"(r3)
			);
	}else if(reg_type == 5)
	{
		__asm__ __volatile__ ( 
			"ldr r3, %0\n\t"
			"sub sp, r11, #4\n\t"
			"ldmfd sp!, {r11, r12}\n\t"
			"bx r8"
			:
			: "m"(r3)
			);
	}else if(reg_type == 6)
	{
		__asm__ __volatile__ ( 
			"ldr r3, %0\n\t"
			"sub sp, r11, #4\n\t"
			"ldmfd sp!, {r11, r12}\n\t"
			"bx r9"
			:
			: "m"(r3)
			);
	}else if(reg_type == 7)
	{
		__asm__ __volatile__ ( 
			"ldr r3, %0\n\t"
			"sub sp, r11, #4\n\t"
			"ldmfd sp!, {r11, r12}\n\t"
			"bx r10"
			:
			: "m"(r3)
			);
	}else if(reg_type == 8)
	{
		__asm__ __volatile__ ( 
			"ldr r3, %0\n\t"
			"sub sp, r11, #4\n\t"
			"ldmfd sp!, {r11, r12}\n\t"
			"bx r2"
			:
			: "m"(r3)
			);
	}else if(reg_type == 9)
	{
		__asm__ __volatile__ ( 
			"ldr r3, %0\n\t"
			"sub sp, r11, #4\n\t"
			"ldmfd sp!, {r11, r12}\n\t"
			"bx r11"
			:
			: "m"(r3)
			);
	}else if(reg_type == 10)
	{
		__asm__ __volatile__ ( 
			"ldr r3, %0\n\t"
			"sub sp, r11, #4\n\t"
			"ldmfd sp!, {r11, r12}\n\t"
			"mov r12, r3\n\t"
			"bx r12"
			:
			: "m"(r12)
			);
	}
}
