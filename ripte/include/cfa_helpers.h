
#ifndef RIPTE_HELPERS_H
#define RIPTE_HELPERS_H

#include <tee_client_api.h>
#include <tee_api_types.h>
#include <string.h>
#include "btbl.h"
#include "ltbl.h"
#include "list.h"

#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>

#define PAGE_SHIFT 12                  // equals to log(page_size)
#define PAGEMAP_LENGTH 8

typedef struct ftbl_entry {
	uint32_t src;
	uint32_t dst;
	uint8_t lr[64];
	uint8_t type;
} ftbl_entry_t;

typedef struct fdtbl_entry {
	uint32_t start;
	uint32_t end;
	uint8_t measurement[32];
	uint8_t type;
} fdtbl_entry_t;

extern struct btbl_entry btbl[];
extern int len_btbl;
extern struct ltbl_entry ltbl[];
extern int len_ltbl;

extern struct ftbl_entry ftbl[];
extern int len_ftbl;
extern struct fdtbl_entry fdtbl[];
extern int len_fdtbl;

/* Global context to use if any context is needed as input to a function */
TEEC_Context ripte_teec_ctx;
TEEC_Result res;
TEEC_Session session;
uint32_t ret_orig;
uint32_t bl_count;
uint32_t blx_count;
uint32_t b_count;
FILE *pagemap;
unsigned int page_size;

TEEC_Result ripte_teec_ctx_init(void);
void ripte_teec_ctx_deinit(void);

/* Opens a session */
TEEC_Result ripte_teec_open_session(TEEC_Session *session, const TEEC_UUID *uuid, TEEC_Operation *op, uint32_t *ret_orig);

TEEC_Result ripte_register_shared_memory(TEEC_SharedMemory shm);

uint32_t get_physical_address(void *addr);
uint64_t get_page_frame_number_of_address(void *addr);

list_t *program_stack;

extern int test_basicmath_small(void);
extern int test_basicmath_large(void);
extern int test_crc_32(int argc, char *argv[]);
extern int test_dijkstra_small(int argc, char *argv[]);
extern int test_dijkstra_large(int argc, char *argv[]);
extern int test_sha(int argc, char *argv[]);
extern int test_string_search_large();
extern int test_string_search_small();
extern int test_cjpeg (int argc, char **argv);
extern int test_djpeg (int argc, char **argv);
extern int test_cubic(void);
extern void test_isqrt(void);
extern void test_rad2deg(void);

#endif