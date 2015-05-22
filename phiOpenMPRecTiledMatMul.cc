#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>

//__attribute__ ((target(mic))) int MIN_BL_SIZE = 128;

__attribute__((target(mic))) void mul(long n, double* a, double* b, double* s, int MIN_BL_SIZE, int num_threads);

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
  
  int MIN_BL_SIZE = 128;

  if(argc > 2)
  {
    MIN_BL_SIZE = atoi(argv[2]);
  }

  printf("Min Block Size: %d\n", MIN_BL_SIZE);

  int num_threads = -1;
  if(argc > 3)
  {
    num_threads = (atoi(argv[3]));
  }

  //omp_set_num_threads(4);
  //TODO Alignment allocations.
  int ok;
  ok = posix_memalign((void**)&a, 64, n*n*sizeof(double));
  ok = posix_memalign((void**)&b, 64, n*n*sizeof(double));
  ok = posix_memalign((void**)&c, 64, n*n*sizeof(double));
  /*
  a = (double*) calloc (n*n,sizeof(double));
  b = (double*) calloc (n*n,sizeof(double));
  c = (double*) calloc (n*n,sizeof(double));
  */
 
  for(long i = 0; i < n*n ; i++ )
  {
    a[i] = i;
    b[i] = i + 1;
  }

#pragma offload target(mic:0) in(a,b:length(n*n)) inout(c:length(n*n)) 
  {
    omp_set_nested(1);
    if(num_threads == -1)
    {
      num_threads = omp_get_max_threads();
    }

    printf("Number of threads: %d\n", num_threads);

    /* Timeval structures store the start and end time of
     * initialization and computation. */
    struct timeval start, end;
    // Start timing vector initialization loop.
    gettimeofday(&start, NULL);

    mul(n, a, b, c, MIN_BL_SIZE, num_threads);

    // End timing vector initialization loop.
    gettimeofday(&end, NULL);
    // Print execution time of vector initialization loop.
    printf ("Multiplication time = %f seconds\n",
        (double) (end.tv_usec - start.tv_usec) / 1000000 +
        (double) (end.tv_sec - start.tv_sec));
       
  }
  printf("n-1th: %f\n", c[n-1]);
}

void mul(long n, double* a, double* b, double* s, int MIN_BL_SIZE, int num_threads)
{
  //printf("threadnum: %d\n", omp_get_thread_num());//TODO
  if (n <= MIN_BL_SIZE)
  {
    for(long i = 0; i < n; i++ ) {
      for(long k = 0; k < n; k++ ) {
        #pragma vector aligned
        #pragma ivdep
        for(long j = 0; j < n; j++ ) {
          s[i*n+j] += a[i*n+k]*b[k*n+j];
        } 
      }
    }
	/*printf("n: %ld, MIN:%d\n", n,MIN_BL_SIZE);
	  for(long i = 0; i < n*n ; i++ )
	  {
		  printf("%f ", s[i]);
	  }
	  printf("\n");
*/
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

    int nxt_lvl_num_thrds[4] = {1, 1, 1, 1};
    int crr_lvl_num_thrds = num_threads;
    if(num_threads >= 4)
    {
      crr_lvl_num_thrds = 4;
      int tmp = num_threads/4;
      for(int i = 0; i < 4; i++)
      {
        nxt_lvl_num_thrds[i] = tmp;
      }
      for(int i = 0; i < num_threads%4; i++)
      {
        nxt_lvl_num_thrds[i]++;
      }
    }
#pragma omp parallel num_threads(crr_lvl_num_thrds)
    {
//#pragma omp single
//    printf("Number of actual threads, lvl: %d, %d\n", omp_get_num_threads(), omp_get_level());
#pragma omp sections
      {
#pragma omp section
        mul(n2, c, g, cgdi, MIN_BL_SIZE, nxt_lvl_num_thrds[0]);
#pragma omp section
        mul(n2, c, h, chdj, MIN_BL_SIZE, nxt_lvl_num_thrds[1]);
#pragma omp section
        mul(n2, e, h, ehfj, MIN_BL_SIZE, nxt_lvl_num_thrds[2]);
#pragma omp section
        mul(n2, e, g, egfi, MIN_BL_SIZE, nxt_lvl_num_thrds[3]);
      }
#pragma omp barrier

#pragma omp sections
      {
#pragma omp section
        mul(n2, d, i, cgdi, MIN_BL_SIZE, nxt_lvl_num_thrds[0]);
#pragma omp section
        mul(n2, d, j, chdj, MIN_BL_SIZE, nxt_lvl_num_thrds[1]);
#pragma omp section
        mul(n2, f, j, ehfj, MIN_BL_SIZE, nxt_lvl_num_thrds[2]);
#pragma omp section
        mul(n2, f, i, egfi, MIN_BL_SIZE, nxt_lvl_num_thrds[3]);
      }
#pragma omp barrier
    }

  }
  //TODO convert to curve and back
}
//TODO Tuning openMP
//TODO Tuning Phi
