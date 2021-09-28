CFLAGS=-O3 -lpthread -g -Wall -pthread -fno-builtin
all:
	gcc memset.c -o memset $(CFLAGS) -lnuma
	gcc memset.c -o memsetpre -DPREPOPULATE $(CFLAGS) -lnuma
	gcc memset.c -o memset.numa -DNUMA $(CFLAGS) -lnuma
	gcc memset.c -o memsetpre.numa -DNUMA -DPREPOPULATE $(CFLAGS) -lnuma

clean:
	rm memset memsetpre memset.numa memsetpre.numa
