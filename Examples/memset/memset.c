/*
 * Micro-test program used to study scalability of Linux kernel code
 * in the case where userspace code is embarrassingly parallel.
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <numa.h>
#include <sched.h>
#include "common.h"

#ifdef PREPOPULATE
#define BENCH_NAME "memsetpre"
#else
#define BENCH_NAME "memset"
#endif

static uint64_t count = 1 << 29; // default: 4 GB

/*
 * Simple thread that just copies in a loop, touching only
 * its own stack page, the source, and the destination, so it doesn't share writable memory
 * with any other thread, and does not call the kernel.
 */
void *do_memset_numa(void *argp)
{
	struct tdata *tdata = (struct tdata *)argp;
	uint64_t *destp;
	size_t size = count * sizeof(uint64_t);

	cpu_pin(tdata->tid % num_proc());
	destp = numa_alloc_onnode(size, numa_node_of_cpu(tdata->tid % num_proc()));
	if (destp == NULL) {
		perror("malloc");
		return (void *) -1;
	} 
#ifdef PREPOPULATE
	else {
		/* to confirm the actual memory allocation */
		size_t i;
		for (i = 0; i < count; i += (4096 / sizeof(uint64_t)))
			destp[i] = 0;	
	}
#endif

	roi_begin();
	memset(destp, 0xcc, size);
	roi_end();

	numa_free(destp, size);

	return NULL;
}

void *do_memset(void *argp)
{
	uint64_t *destp;
	size_t size = count * sizeof(uint64_t);

	//cpu_pin(tdata->tid % num_proc());
	destp = malloc(size);
	if (destp == NULL) {
		perror("malloc");
		return (void *) -1;
	} 
#ifdef PREPOPULATE
	else {
		/* to confirm the actual memory allocation */
		size_t i;
		for (i = 0; i < count; i += (4096 / sizeof(uint64_t)))
			destp[i] = 0;	
	}
#endif

	roi_begin();
	memset(destp, 0xcc, size);
	roi_end();

	free(destp);

	return NULL;
}
int cleanup_memset(int threads, struct tdata **tdata) {
	return 0;
}

int main(int argc, char *argv[])
{
	uint64_t threads;
	double duration;
	struct tdata **tdata;
	
	threads = num_proc();

	if (parse_option(argc, argv, &threads, &count)) {
		fprintf(stderr, "Usage: %s {threads} {count}\n", argv[0]);
		return -1;
	}

#ifdef NUMA
	cpu_pin(0);	
#endif

	init_benchmark(threads);
#ifdef NUMA
	tdata = init_tdata_numa();
#else
	tdata = init_tdata();
#endif
	if (!tdata) return -1;

	fprintf(stderr, "Starting %lu threads\n", threads);
#ifdef NUMA
	duration = run_threads(tdata, do_memset_numa);
#else
	duration = run_threads(tdata, do_memset);
#endif
	fprintf(stderr, "Threads stopped\n\n");
	fprintf(stderr, "Filled destination in %lu threads in %f sec.\n", threads,duration);
	fprintf(stderr, "Achieving parallel memory read-write bandwidth: %f GB/sec.\n", (count * sizeof(uint64_t) / duration / 1e9) * threads * 2.0);

	cleanup_memset(threads, tdata);
#ifdef NUMA
	free_tdata_numa(tdata);
#else
	free_tdata(tdata);
#endif
	
	print_result(BENCH_NAME, threads, duration, count);

	return 0;
}
