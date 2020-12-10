.PHONY: all
all: clpperfchk clean

clpperfchk: clpperfchk.o 
	cc -o clpperfchk clpperfchk.o
clpperfchk.o: perfchk.c
	cc -c perfchk.c -o clpperfchk.o

clpmdperf.o: perfchk.h

.PHONY: clean
clean:
	rm -f clpperfchk.o 
