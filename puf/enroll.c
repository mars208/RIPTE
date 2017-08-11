#include <math.h>
#include <stdio.h>
#include "bch3.h"

int
generatePS(){
  int i;
  for (i = 0; i < k; i++)
    data[i] = ( random() & 65536 ) >> 16;

  encode_bch();           /* encode data */
  /*
   * recd[] are the coefficients of c(x) = x**(length-k)*data(x) + b(x)
   */
  for (i = 0; i < codelength - k; i++)
    recd[i] = bb[i];
  for (i = 0; i < k; i++)
    recd[i + codelength - k] = data[i];

  printf("Primary seed is:\n");
  for(i = 0; i < k; i++){
    printf("%d", data[i]);
  }
  printf("\n");

  printf("The Code is:\n");
  for (i = 0; i < codelength; i++) {
    printf("%1d,", recd[i]);
  }
  printf("\n");

  return 0;
}


int
main(){
  int             i;

  read_p();               /* Read m */
  generate_gf();          /* Construct the Galois Field GF(2**m) */
  gen_poly();             /* Compute the generator polynomial of BCH code */

  /* Randomly generate Primary Seed*/
  seed = 131073;
  srandom(seed);
 
  for( i = 0; i < 6; i++){
    generatePS();
  }

  return 0;
}
