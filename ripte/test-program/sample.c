#include "ripte_stub.h"
#include "ripte_helpers.h"
#include "log.h"
#include "trampoline.h"
#include "stdio.h"
#include "stdlib.h"

static uint8_t user_data[8] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
static uint8_t quote_out[128];
static uint32_t quote_len;

int testdiv()
{
	// test_basicmath_small();
	// test_basicmath_large();
	
	for (int i = 0; i < 1000; ++i)
	{	
		test_cubic();
	}
	// test_isqrt();
	// test_rad2deg();
	
	// char crc_arg_str1[] = "crc";
	// char crc_arg_str2[] = "/bin/cflat_data/large.pcm";
	// char *crc_arg[] = {crc_arg_str1, crc_arg_str2};
	// test_crc_32(2, crc_arg);

	// char dijkstra_arg_str1[] = "dijkstra";
	// char dijkstra_arg_str2[] = "/bin/cflat_data/input.dat";
	// char *dijkstra_arg[] = {dijkstra_arg_str1, dijkstra_arg_str2};
	// test_dijkstra_small(2, dijkstra_arg);
	// test_dijkstra_large(2, dijkstra_arg);

	// char sha_arg_str1[] = "sha";
	// char sha_arg_str2[] = "/bin/cflat_data/input_small.asc";
	// char sha_arg_str3[] = "/bin/cflat_data/input_large.asc";
	// char *sha_arg_small[] = {sha_arg_str1, sha_arg_str2};
	// char *sha_arg_large[] = {sha_arg_str1, sha_arg_str3};
	// test_sha(2, sha_arg_small);
	// test_sha(2, sha_arg_large);

	// test_string_search_large();
	// test_string_search_small();

	// char *jpeg_arg_small1[] = {"jpeg-6a/cjpeg", "-dct", "int", "-progressive", "-opt", "-outfile", "/bin/cflat_data/output_small_encode.jpeg", "/bin/cflat_data/input_small.ppm"};
	// char *jpeg_arg_small2[] = {"jpeg-6a/djpeg", "-dct", "int", "-ppm", "-outfile", "/bin/cflat_data/output_small_decode.ppm", "/bin/cflat_data/input_small.jpg"};
	// char *jpeg_arg_large1[] = {"jpeg-6a/cjpeg", "-dct", "int", "-progressive", "-opt", "-outfile", "/bin/cflat_data/output_large_encode.jpeg", "/bin/cflat_data/input_large.ppm"};
	// char *jpeg_arg_large2[] = {"jpeg-6a/djpeg", "-dct", "int", "-ppm", "-outfile", "/bin/cflat_data/output_large_decode.ppm", "/bin/cflat_data/input_large.jpg"};
	// test_djpeg(7, jpeg_arg_small2);
	// test_cjpeg(8, jpeg_arg_small1);
	// test_cjpeg(8, jpeg_arg_large1);
	// test_djpeg(7, jpeg_arg_large2);

	// quickSort(0,MAX-1);
	
	return 0;
}


void testfunction(){
	struct  timeval  start;
	struct  timeval  end;
	unsigned long timer;

   // Open the pagemap file for the current process
    pagemap = fopen("/proc/self/pagemap", "rb");
    page_size = getpagesize();
	uint32_t physcial_addr1 = get_physical_address(&testdiv);
	uint32_t physcial_addr2 = get_physical_address(&testfunction);
   // Open the pagemap file for the current process
	pagemap = fopen("/proc/self/pagemap", "rb");

	ripte_init((ripte_addr_t)physcial_addr1, (ripte_addr_t)physcial_addr2);

	program_stack = (list_t *)malloc(sizeof(list_t));
	list_init(program_stack);
	bl_count = 0;
	blx_count = 0;

	gettimeofday(&start,NULL);
	testdiv();
	gettimeofday(&end,NULL);
	timer = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
 	printf("timer = %ld us\n",timer);
 	printf("bl_count:%d, blx_count:%d, b_count:%d\n", bl_count, blx_count, b_count);
 	
	quote_len = sizeof(quote_out);
	ripte_quote(user_data, sizeof(user_data), quote_out, &quote_len);
    fclose(pagemap);

}


int main(int argc, char const *argv[])
{
	printf("Starting testing \n");
	testfunction();
	return 0;
}

void func(char *str, char *m) {
	char *i = m;
	int (*func_main)(int argc, char const *argv[]);
	char buf[126];
	func_main = &main;
	strcpy(buf,str);
	*i = buf[0];
	func_main(1, buf[0]);
	func2(buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
}

void func2(char str, char str1, char str2, char str3, char str4, char str5) {
	char sss;
	sss = str ^ str1 ^ str2 ^ str3 ^ str4 ^ str5;
	return;
}
