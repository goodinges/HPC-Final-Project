#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>

int main(int argc, char *argv[])
{
  long n;

  double* a;
  double* b;
  double* c;

  if(argc < 2)
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
  
  printf("%ldx%ld Matrix\n", n, n);
  
  int ok;

  ok = posix_memalign((void**)&a, 64, n*n*sizeof(double));
  ok = posix_memalign((void**)&b, 64, n*n*sizeof(double));
  ok = posix_memalign((void**)&c, 64, n*n*sizeof(double));
 
  for(long i = 0; i < n*n ; i++ )
  {
    a[i] = i;
    b[i] = i + 1;
  }

  {
    /* Timeval structures store the start and end time of
     * initialization and computation. */
    struct timeval start, end;
    // Start timing vector initialization loop.
    gettimeofday(&start, NULL);

    for(long i = 0; i < n; i++ ) {
      for(long k = 0; k < n; k++ ) {
        #pragma vector aligned
        #pragma ivdep
        for(long j = 0; j < n; j++ ) {
          c[i*n+j] += a[i*n+k]*b[k*n+j];
        } 
      }
    }
/*	printf("n: %ld\n", n);
	  for(long i = 0; i < n*n ; i++ )
	  {
		  printf("%f ", c[i]);
	  }
	  printf("\n");
*/

    // End timing vector initialization loop.
    gettimeofday(&end, NULL);
    // Print execution time of vector initialization loop.
    printf ("Multiplication time = %f seconds\n",
        (double) (end.tv_usec - start.tv_usec) / 1000000 +
        (double) (end.tv_sec - start.tv_sec));
       
  }
  printf("n-1th: %f\n", c[n-1]);
}

