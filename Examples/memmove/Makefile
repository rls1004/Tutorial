CFLAGS=-O3 -lpthread -g -Wall -pthread -fno-builtin
all:
	gcc memmove.c -o memmove $(CFLAGS) -lnuma
	gcc memmove.c -o memmovepre -DPREPOPULATE $(CFLAGS) -lnuma
	gcc memmove.c -o memmove.numa -DNUMA $(CFLAGS) -lnuma
	gcc memmove.c -o memmovepre.numa -DNUMA -DPREPOPULATE $(CFLAGS) -lnuma

clean:
	rm memmove memmovepre memmove.numa memmovepre.numa
