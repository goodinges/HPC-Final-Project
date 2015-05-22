all: openmp phi
openmp: phiOpenMPRecTiledMatMul.cc
	 icpc -openmp -openmp_report2 -xhost -O3 -offload-option,mic,compiler,"-O2 -fma" -opt-report-phase=offload -vec-report2 -no-offload -o openMPRecTiledMatMul phiOpenMPRecTiledMatMul.cc
phi: phiOpenMPRecTiledMatMul.cc
	 icpc -openmp -openmp_report2 -xhost -O3 -offload-option,mic,compiler,"-O2 -fma" -opt-report-phase=offload -vec-report2 -o phiOpenMPRecTiledMatMul phiOpenMPRecTiledMatMul.cc

mkl: mkl.cc
	icpc -openmp -openmp_report2 -xhost -O3 -offload-option,mic,compiler,"-O2 -fma" -opt-report-phase=offload -vec-report2 -mkl -o mkl mkl.cc
#serial: serialRecTiledMatMul.cc
#	 icpc -openmp -openmp_report2 -xhost -O3 -offload-option,mic,compiler,"-O2 -fma" -opt-report-phase=offload -vec-report2 -o serialRecTiledMatMul serialRecTiledMatMul.cc
