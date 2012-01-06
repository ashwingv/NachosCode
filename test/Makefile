# use normal make for this Makefile
#
# Makefile for building user programs to run on top of Nachos
#
# Several things to be aware of:
#
#    Nachos assumes that the location of the program startup routine (the
# 	location the kernel jumps to when the program initially starts up)
#       is at location 0.  This means: start.o must be the first .o passed 
# 	to ld, in order for the routine "Start" to be loaded at location 0
#

# If you are running on SunOS 5.x machines, uncomment the next line
GCCDIR = /auto/home-scf-22/csci402/testgnu/decstation-ultrix/bin/
LDFLAGS = -T newscript -N
ASFLAGS = -mips1
CPPFLAGS = $(INCDIR)

# if you aren't cross-compiling:
# GCCDIR =
# LDFLAGS = -N -T 0
# ASFLAGS =
# CPPFLAGS = -P $(INCDIR)


CC = $(GCCDIR)gcc
AS = $(GCCDIR)as
LD = $(GCCDIR)ld
CPP = $(GCCDIR)cpp

INCDIR =-I../userprog -I../threads
CFLAGS = -G 0 -c $(INCDIR)

all: halt matmult sort testfiles locktest1 locktest2 locktest3 locktest4 locktest5 locktest6 cvtest1 cvtest2 cvtest3 cvtest4 cvtest5 cvtest6 cvtest7 forktest1 forktest2 exectest1 museum museum1 museum2 museum3 manymuseum execMatmult execSort execMatMultSort forkMatmult forkSort forkMatmultSort testRpcLock testRpcLock1 testRpcLock2 testRpcLock3 testRpcCondition testRpcCondition1 testRpcCondition2 testRpcCondition3

start.o: start.s ../userprog/syscall.h
	$(CPP) $(CPPFLAGS) start.s > strt.s
	$(AS) $(ASFLAGS) -o start.o strt.s
	rm strt.s

halt.o: halt.c
	$(CC) $(CFLAGS) -c halt.c
halt: halt.o start.o
	$(LD) $(LDFLAGS) start.o halt.o -o halt.coff
	../bin/coff2noff halt.coff halt

sort.o: sort.c
	$(CC) $(CFLAGS) -c sort.c
sort: sort.o start.o
	$(LD) $(LDFLAGS) start.o sort.o -o sort.coff
	../bin/coff2noff sort.coff sort

matmult.o: matmult.c
	$(CC) $(CFLAGS) -c matmult.c
matmult: matmult.o start.o
	$(LD) $(LDFLAGS) start.o matmult.o -o matmult.coff
	../bin/coff2noff matmult.coff matmult

testfiles.o: testfiles.c
	$(CC) $(CFLAGS) -c testfiles.c
testfiles: testfiles.o start.o
	$(LD) $(LDFLAGS) start.o testfiles.o -o testfiles.coff
	../bin/coff2noff testfiles.coff testfiles

createLockTest.o: createLockTest.c
	$(CC) $(CFLAGS) -c createLockTest.c
createLockTest: createLockTest.o start.o
	$(LD) $(LDFLAGS) start.o createLockTest.o -o createLockTest.coff
	../bin/coff2noff createLockTest.coff createLockTest

testExec.o: testExec.c
	$(CC) $(CFLAGS) -c testExec.c
testExec: testExec.o start.o
	$(LD) $(LDFLAGS) start.o testExec.o -o testExec.coff
	../bin/coff2noff testExec.coff testExec

testFork.o: testFork.c
	$(CC) $(CFLAGS) -c testFork.c
testFork: testFork.o start.o
	$(LD) $(LDFLAGS) start.o testFork.o -o testFork.coff
	../bin/coff2noff testFork.coff testFork

lockTest.o: lockTest.c
	$(CC) $(CFLAGS) -c lockTest.c
lockTest: lockTest.o start.o
	$(LD) $(LDFLAGS) start.o lockTest.o -o lockTest.coff
	../bin/coff2noff lockTest.coff lockTest

exitTest.o: exitTest.c
	$(CC) $(CFLAGS) -c exitTest.c
exitTest: exitTest.o start.o
	$(LD) $(LDFLAGS) start.o exitTest.o -o exitTest.coff
	../bin/coff2noff exitTest.coff exitTest


locktest1.o: locktest1.c
	$(CC) $(CFLAGS) -c locktest1.c
locktest1: locktest1.o start.o
	$(LD) $(LDFLAGS) start.o locktest1.o -o locktest1.coff
	../bin/coff2noff locktest1.coff locktest1

locktest2.o: locktest2.c
	$(CC) $(CFLAGS) -c locktest2.c
locktest2: locktest2.o start.o
	$(LD) $(LDFLAGS) start.o locktest2.o -o locktest2.coff
	../bin/coff2noff locktest2.coff locktest2

locktest4.o: locktest4.c
	$(CC) $(CFLAGS) -c locktest4.c
locktest4: locktest4.o start.o
	$(LD) $(LDFLAGS) start.o locktest4.o -o locktest4.coff
	../bin/coff2noff locktest4.coff locktest4


locktest3.o: locktest3.c
	$(CC) $(CFLAGS) -c locktest3.c
locktest3: locktest3.o start.o
	$(LD) $(LDFLAGS) start.o locktest3.o -o locktest3.coff
	../bin/coff2noff locktest3.coff locktest3

locktest5.o: locktest5.c
	$(CC) $(CFLAGS) -c locktest5.c
locktest5: locktest5.o start.o
	$(LD) $(LDFLAGS) start.o locktest5.o -o locktest5.coff
	../bin/coff2noff locktest5.coff locktest5

locktest6.o: locktest6.c
	$(CC) $(CFLAGS) -c locktest6.c
locktest6: locktest6.o start.o
	$(LD) $(LDFLAGS) start.o locktest6.o -o locktest6.coff
	../bin/coff2noff locktest6.coff locktest6

cvtest1.o: cvtest1.c
	$(CC) $(CFLAGS) -c cvtest1.c
cvtest1: cvtest1.o start.o
	$(LD) $(LDFLAGS) start.o cvtest1.o -o cvtest1.coff
	../bin/coff2noff cvtest1.coff cvtest1

cvtest2.o: cvtest2.c
	$(CC) $(CFLAGS) -c cvtest2.c
cvtest2: cvtest2.o start.o
	$(LD) $(LDFLAGS) start.o cvtest2.o -o cvtest2.coff
	../bin/coff2noff cvtest2.coff cvtest2

cvtest3.o: cvtest3.c
	$(CC) $(CFLAGS) -c cvtest3.c
cvtest3: cvtest3.o start.o
	$(LD) $(LDFLAGS) start.o cvtest3.o -o cvtest3.coff
	../bin/coff2noff cvtest3.coff cvtest3

cvtest4.o: cvtest4.c
	$(CC) $(CFLAGS) -c cvtest4.c
cvtest4: cvtest4.o start.o
	$(LD) $(LDFLAGS) start.o cvtest4.o -o cvtest4.coff
	../bin/coff2noff cvtest4.coff cvtest4

cvtest5.o: cvtest5.c
	$(CC) $(CFLAGS) -c cvtest5.c
cvtest5: cvtest5.o start.o
	$(LD) $(LDFLAGS) start.o cvtest5.o -o cvtest5.coff
	../bin/coff2noff cvtest5.coff cvtest5

cvtest6.o: cvtest6.c
	$(CC) $(CFLAGS) -c cvtest6.c
cvtest6: cvtest6.o start.o
	$(LD) $(LDFLAGS) start.o cvtest6.o -o cvtest6.coff
	../bin/coff2noff cvtest6.coff cvtest6

cvtest7.o: cvtest7.c
	$(CC) $(CFLAGS) -c cvtest7.c
cvtest7: cvtest7.o start.o
	$(LD) $(LDFLAGS) start.o cvtest7.o -o cvtest7.coff
	../bin/coff2noff cvtest7.coff cvtest7


forktest1.o: forktest1.c
	$(CC) $(CFLAGS) -c forktest1.c
forktest1: forktest1.o start.o
	$(LD) $(LDFLAGS) start.o forktest1.o -o forktest1.coff
	../bin/coff2noff forktest1.coff forktest1

forktest2.o: forktest2.c
	$(CC) $(CFLAGS) -c forktest2.c
forktest2: forktest2.o start.o
	$(LD) $(LDFLAGS) start.o forktest2.o -o forktest2.coff
	../bin/coff2noff forktest2.coff forktest2

exectest1.o: exectest1.c
	$(CC) $(CFLAGS) -c exectest1.c
exectest1: exectest1.o start.o
	$(LD) $(LDFLAGS) start.o exectest1.o -o exectest1.coff
	../bin/coff2noff exectest1.coff exectest1

museum.o: museum.c
	$(CC) $(CFLAGS) -c museum.c
museum: museum.o start.o
	$(LD) $(LDFLAGS) start.o museum.o -o museum.coff
	../bin/coff2noff museum.coff museum

museum1.o: museum1.c
	$(CC) $(CFLAGS) -c museum1.c
museum1: museum1.o start.o
	$(LD) $(LDFLAGS) start.o museum1.o -o museum1.coff
	../bin/coff2noff museum1.coff museum1

museum2.o: museum2.c
	$(CC) $(CFLAGS) -c museum2.c
museum2: museum2.o start.o
	$(LD) $(LDFLAGS) start.o museum2.o -o museum2.coff
	../bin/coff2noff museum2.coff museum2

museum3.o: museum3.c
	$(CC) $(CFLAGS) -c museum3.c
museum3: museum3.o start.o
	$(LD) $(LDFLAGS) start.o museum3.o -o museum3.coff
	../bin/coff2noff museum3.coff museum3

manymuseum.o: manymuseum.c
	$(CC) $(CFLAGS) -c manymuseum.c
manymuseum: manymuseum.o start.o
	$(LD) $(LDFLAGS) start.o manymuseum.o -o manymuseum.coff
	../bin/coff2noff manymuseum.coff manymuseum

execMatmult.o: execMatmult.c
	$(CC) $(CFLAGS) -c execMatmult.c
execMatmult: execMatmult.o start.o
	$(LD) $(LDFLAGS) start.o execMatmult.o -o execMatmult.coff
	../bin/coff2noff execMatmult.coff execMatmult


execSort.o: execSort.c
	$(CC) $(CFLAGS) -c execSort.c
execSort: execSort.o start.o
	$(LD) $(LDFLAGS) start.o execSort.o -o execSort.coff
	../bin/coff2noff execSort.coff execSort

execMatMultSort.o: execMatMultSort.c
	$(CC) $(CFLAGS) -c execMatMultSort.c
execMatMultSort: execMatMultSort.o start.o
	$(LD) $(LDFLAGS) start.o execMatMultSort.o -o execMatMultSort.coff
	../bin/coff2noff execMatMultSort.coff execMatMultSort

forkMatmult.o: forkMatmult.c
	$(CC) $(CFLAGS) -c forkMatmult.c
forkMatmult: forkMatmult.o start.o
	$(LD) $(LDFLAGS) start.o forkMatmult.o -o forkMatmult.coff
	../bin/coff2noff forkMatmult.coff forkMatmult


forkSort.o: forkSort.c
	$(CC) $(CFLAGS) -c forkSort.c
forkSort: forkSort.o start.o
	$(LD) $(LDFLAGS) start.o forkSort.o -o forkSort.coff
	../bin/coff2noff forkSort.coff forkSort


forkMatmultSort.o: forkMatmultSort.c
	$(CC) $(CFLAGS) -c forkMatmultSort.c
forkMatmultSort: forkMatmultSort.o start.o
	$(LD) $(LDFLAGS) start.o forkMatmultSort.o -o forkMatmultSort.coff
	../bin/coff2noff forkMatmultSort.coff forkMatmultSort


testRpcLock.o: testRpcLock.c
	$(CC) $(CFLAGS) -c testRpcLock.c
testRpcLock: testRpcLock.o start.o
	$(LD) $(LDFLAGS) start.o testRpcLock.o -o testRpcLock.coff
	../bin/coff2noff testRpcLock.coff testRpcLock

testRpcLock1.o: testRpcLock1.c
	$(CC) $(CFLAGS) -c testRpcLock1.c
testRpcLock1: testRpcLock1.o start.o
	$(LD) $(LDFLAGS) start.o testRpcLock1.o -o testRpcLock1.coff
	../bin/coff2noff testRpcLock1.coff testRpcLock1

testRpcLock2.o: testRpcLock2.c
	$(CC) $(CFLAGS) -c testRpcLock2.c
testRpcLock2: testRpcLock2.o start.o
	$(LD) $(LDFLAGS) start.o testRpcLock2.o -o testRpcLock2.coff
	../bin/coff2noff testRpcLock2.coff testRpcLock2

testRpcLock3.o: testRpcLock3.c
	$(CC) $(CFLAGS) -c testRpcLock3.c
testRpcLock3: testRpcLock3.o start.o
	$(LD) $(LDFLAGS) start.o testRpcLock3.o -o testRpcLock3.coff
	../bin/coff2noff testRpcLock3.coff testRpcLock3


testRpcCondition.o: testRpcCondition.c
	$(CC) $(CFLAGS) -c testRpcCondition.c
testRpcCondition: testRpcCondition.o start.o
	$(LD) $(LDFLAGS) start.o testRpcCondition.o -o testRpcCondition.coff
	../bin/coff2noff testRpcCondition.coff testRpcCondition

testRpcCondition1.o: testRpcCondition1.c
	$(CC) $(CFLAGS) -c testRpcCondition1.c
testRpcCondition1: testRpcCondition1.o start.o
	$(LD) $(LDFLAGS) start.o testRpcCondition1.o -o testRpcCondition1.coff
	../bin/coff2noff testRpcCondition1.coff testRpcCondition1

testRpcCondition2.o: testRpcCondition2.c
	$(CC) $(CFLAGS) -c testRpcCondition2.c
testRpcCondition2: testRpcCondition2.o start.o
	$(LD) $(LDFLAGS) start.o testRpcCondition2.o -o testRpcCondition2.coff
	../bin/coff2noff testRpcCondition2.coff testRpcCondition2

testRpcCondition3.o: testRpcCondition3.c
	$(CC) $(CFLAGS) -c testRpcCondition3.c
testRpcCondition3: testRpcCondition3.o start.o
	$(LD) $(LDFLAGS) start.o testRpcCondition3.o -o testRpcCondition3.coff
	../bin/coff2noff testRpcCondition3.coff testRpcCondition3

clean:
	rm -f *.o *.coff
