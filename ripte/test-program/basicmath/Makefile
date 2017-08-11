# FILE1 = basicmath_small.c   rad2deg.c  cubic.c   isqrt.c
# FILE2 = basicmath_large.c   rad2deg.c  cubic.c   isqrt.c

objs = basicmath_small.o basicmath_large.o rad2deg.o cubic.o isqrt.o

all: $(objs)

# basicmath_small: ${FILE1} Makefile
# 	# gcc -static -O3 ${FILE1} -o basicmath_small -lm
# 	$(CC) $(LDADD) -o $@ $^

# basicmath_large: ${FILE2} Makefile
# 	# gcc -static -O3 ${FILE2} -o basicmath_large -lm
# 	$(CC) $(LDADD) -o $@ $^

clean:
	rm -rf basicmath_small basicmath_large output*
