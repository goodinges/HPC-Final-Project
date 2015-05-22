# HPC-Final-Project


Best min block: 512

c557-804.stampede(22)$ ./openMPRecTiledMatMul 2048 512 8
2048x2048 Matrix
Min Block Size: 512
Number of threads: 8
Multiplication time = 0.928154 seconds
n-1th: 3137066482270208.000000
c557-804.stampede(23)$ ./openMPRecTiledMatMul 2048 512 1
2048x2048 Matrix
Min Block Size: 512
Number of threads: 1
Multiplication time = 4.411639 seconds
n-1th: 3137066482270208.000000
c558-802.stampede(40)$ time ./phiOpenMPRecTiledMatMul 2048 256 240
2048x2048 Matrix
Min Block Size: 256
n-1th: 3192107517345792.000000
Number of threads: 240
Multiplication time = 0.654591 seconds

real	0m2.637s
user	0m0.698s
sys	0m0.132s


c557-804.stampede(35)$ ./openMPRecTiledMatMul 8192 512 8
8192x8192 Matrix
Min Block Size: 512
Number of threads: 8
Multiplication time = 50.721080 seconds
n-1th: 3276864075362992128.000000
c557-804.stampede(36)$ ./openMPRecTiledMatMul 8192 512 1
8192x8192 Matrix
Min Block Size: 512
Number of threads: 1
Multiplication time = 280.974493 seconds
n-1th: 3276864075362992128.000000
c558-802.stampede(51)$ time ./phiOpenMPRecTiledMatMul 8192 128 240
8192x8192 Matrix
Min Block Size: 128
n-1th: 3280666264553390080.000000
Number of threads: 240
Multiplication time = 15.163846 seconds

real	0m24.454s
user	0m15.301s
sys	0m1.771s

