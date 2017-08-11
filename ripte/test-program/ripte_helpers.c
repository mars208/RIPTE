#include "ripte_helpers.h"

TEEC_Result ripte_teec_ctx_init(void)
{
	return TEEC_InitializeContext(NULL, &ripte_teec_ctx);
}

TEEC_Result ripte_teec_open_session(TEEC_Session *session,
				    const TEEC_UUID *uuid, TEEC_Operation *op,
				    uint32_t *ret_orig)
{
	return TEEC_OpenSession(&ripte_teec_ctx, session, uuid,
				TEEC_LOGIN_PUBLIC, NULL, op, ret_orig);
}

TEEC_Result ripte_register_shared_memory(TEEC_SharedMemory shm)
{
	return TEEC_RegisterSharedMemory(&ripte_teec_ctx, &shm);
}
void ripte_teec_ctx_deinit(void)
{
	/*TEEC_ReleaseSharedMemory(&shmBtbl);
	TEEC_ReleaseSharedMemory(&shmLtbl);*/
	TEEC_FinalizeContext(&ripte_teec_ctx);
}


uint32_t get_physical_address(void *addr){
	unsigned int distance_from_page_boundary = (unsigned long)addr % page_size;
	uint64_t page_frame_number = get_page_frame_number_of_address(addr);
	uint32_t physcial_addr = (page_frame_number << PAGE_SHIFT) + distance_from_page_boundary;
	return physcial_addr;
}

uint64_t get_page_frame_number_of_address(void *addr) {

   // Seek to the page that the buffer is on
   unsigned long offset = (unsigned long)addr / page_size * PAGEMAP_LENGTH;
   if(fseek(pagemap, (unsigned long)offset, SEEK_SET) != 0) {
      fprintf(stderr, "Failed to seek pagemap to proper location\n");
      exit(1);
   }

   // The page frame number is in bits 0-54 so read the first 7 bytes and clear the 55th bit
   uint64_t page_frame_number = 0;
   fread(&page_frame_number, 1, PAGEMAP_LENGTH, pagemap);

   page_frame_number &= 0x7FFFFFFFFFFFFF;

   return page_frame_number;
}