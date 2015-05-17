#include <stdio.h>
#include <stdlib.h>

#define MIN_BL_SIZE  2

void mul(long n, double* a, double* b, double* s);

int main(int argc, char *argv[])
{
  long n;

  double* a;
  double* b;
  double* c;

  if(argc != 2)
  {
    fprintf(stderr, "Please specify the dimention of the matrices.\n");
    return -1;
  }

  n = atol(argv[1]);

  if(n == 0 || (n & (n - 1)) != 0)
  {
    fprintf(stderr, "The dimention of the matrices must be power of two.\n");
    return -1;
  }
  
  //TODO Alignment allocations.
  a = (double*) calloc (n*n,sizeof(double));
  b = (double*) calloc (n*n,sizeof(double));
  c = (double*) calloc (n*n,sizeof(double));
 
  for(long i = 0; i < n*n ; i++ )
  {
    a[i] = i;
    b[i] = i + 1;
  }
  mul(n, a, b, c);
  for(long i = 0; i < n*n ; i++ )
  {
    printf("%f ", c[i]);
  }
  printf("\n");
}

void mul(long n, double* a, double* b, double* s)
{
  if (n <= MIN_BL_SIZE)
  {
    for(long i = 0; i < n; i++ ) {
      for(long k = 0; k < n; k++ ) {
        //#pragma vector aligned
        //#pragma ivdep
        for(long j = 0; j < n; j++ ) {
          s[i*n+j] += a[i*n+k]*b[k*n+j];
        } 
      }
    }
  } else {
    long n2 = n/2;
    long offset = (n*n)/4;
    
    double *c, *d, *e, *f, *g, *h, *i, *j;
    double *cgdi, *chdj, *egfi, *ehfj;

    c = a;
    d = c + offset;
    e = d + offset;
    f = e + offset;

    g = b;
    h = g + offset;
    i = h + offset;
    j = i + offset;

    cgdi = s;
    chdj = cgdi + offset;
    egfi = chdj + offset;
    ehfj = egfi + offset;

    mul(n2, c, g, cgdi);
    mul(n2, c, h, chdj);
    mul(n2, e, h, ehfj);
    mul(n2, e, g, egfi);
    //sync

    mul(n2, d, i, cgdi);
    mul(n2, d, j, chdj);
    mul(n2, f, j, ehfj);
    mul(n2, f, i, egfi);
    //sync


  }
  //TODO convert to curve and back
}
