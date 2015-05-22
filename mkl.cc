#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>
#include <mkl.h>

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
  
  if(argc < 3)
  {
    fprintf(stderr, "Please specify to force MKL offload(2), disable MKL offload(0) or automate MKL offload(1).\n");
  }

  int mkl_mic = atoi(argv[2]);
  if(mkl_mic==1)
  {
    mkl_mic_enable();
  }else if(mkl_mic==2)
  {
    mkl_mic_enable();
    mkl_mic_set_workdivision(MKL_TARGET_MIC, 0, 1);
  }else if(mkl_mic==0)
  {
    mkl_mic_disable();
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

    double wd;
    mkl_mic_get_workdivision(MKL_TARGET_MIC, 0, &wd);
    printf("proportion: %f\n", wd);
    cblas_dgemm(CblasRowMajor, CblasNoTrans,  CblasNoTrans, n, n, n, 1.00, a, n, b, n, 0, c, n);

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


