Compiling:
Shared object for JNI:
gcc -fPIC -fopenmp -O3 -o libecoSpace.so -shared -I/usr/lib/jvm/java-7-openjdk-amd64/include -B tiff-4.0.3/libtiff/.libs extract-vars.c distancequery.c distances.c readtiffs.c kernel-dens.c build-kernel.c -lc -ltiff

Stand-alone program for PNG generation:
gcc -O3 -o get-density-png -B lpng1614 get-density-png.c build-kernel.c -lpng -lm -lz
