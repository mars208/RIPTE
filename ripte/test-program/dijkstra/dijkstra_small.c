#include <stdio.h>

#define NUM_NODES                          100
#define NONE                               9999

struct _NODE
{
  int iDist;
  int iPrev;
};
typedef struct _NODE NODE;

struct _QITEM
{
  int iNode;
  int iDist;
  int iPrev;
  struct _QITEM *qNext;
};
typedef struct _QITEM QITEM;

int AdjMatrix[NUM_NODES][NUM_NODES];

NODE rgnNodes[NUM_NODES];
int ch;
int iPrev, iNode;
int i, iCost, iDist;

int test_dijkstra_small(int argc, char *argv[]) {
  int i,j,k;
  FILE *fp;
  
  if (argc<2) {
    fprintf(stderr, "Usage: dijkstra <filename>\n");
    fprintf(stderr, "Only supports matrix size is #define'd.\n");
  }

  /* open the adjacency matrix file */
  fp = fopen (argv[1],"r");

  /* make a fully connected matrix */
  for (i=0;i<NUM_NODES;i++) {
    for (j=0;j<NUM_NODES;j++) {
      /* make it more sparce */
      fscanf(fp,"%d",&k);
			AdjMatrix[i][j]= k;
    }
  }

  /* finds 10 shortest paths between nodes */
  for (i=0,j=NUM_NODES/2;i<20;i++,j++) {
			j=j%NUM_NODES;
      dijkstra(i,j);
  }
  return 0;
  

}
