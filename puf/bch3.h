#ifndef BCH3_H
#define BCH3_H

void read_p();
void generate_gf();
void gen_poly();
void encode_bch();
void decode_bch();

extern int m, n, codelength, k, t, d;
extern int p[];
extern int alpha_to[], index_of[], g[];
extern int recd[], data[], bb[];
extern int seed;
extern int numerr, errpos[], decerror;
#endif
