/*
        This file is part of the linuxapi project.
        Copyright (C) 2011-2013 Mark Veltzer <mark.veltzer@gmail.com>

        The linuxapi package is free software; you can redistribute it and/or
        modify it under the terms of the GNU Lesser General Public
        License as published by the Free Software Foundation; either
        version 2.1 of the License, or (at your option) any later version.

        The linuxapi package is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
        Lesser General Public License for more details.

        You should have received a copy of the GNU Lesser General Public
        License along with the GNU C Library; if not, write to the Free
        Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
        02111-1307 USA.
 */

#include <firstinclude.h>
#include <sys/types.h>	// for getpid(2), ftruncate(2)
#include <unistd.h>	// for getpid(2), sleep(3), nice(2), fork(2),
			//ftruncate(2)
#include <sched.h>	// for sched_setaffinity(2), CPU_ZERO(3), CPU_SET(3)
#include <stdio.h>	// for printf(3), fflush(3), fprintf(3)
#include <sys/mman.h>	// for shm_open(3), shm_unlink(3)
#include <sys/stat.h>	// for shm_open(3), shm_unlink(3)
#include <fcntl.h>	// for shm_open(3), shm_unlink(3)
#include <stdlib.h>	// for EXIT_SUCCESS, atoi(3), EXIT_FAILURE
#include <us_helper.h>	// for CHECK_ZERO(), CHECK_NOT_M1(), TRACE()

/*
 * This examples shows the effect of the nice scheduling system (SCHED_OTHER).
 *
 * It binds the process to a single core (for multi core systems).
 * It then forks several versions of heavy work each with a different nice
 *level.
 * Then you can compare the work done by each of these.
 *
 * EXTRA_LINK_FLAGS=-lrt
 *
 * References:
 * man 2 nice
 */

void make_child(const int niceval, int* const prog, int core) {
	pid_t pid=CHECK_NOT_M1(fork());
	if(pid!=0) {	// parent
		return;
	}
	// set the nice value for myself
	CHECK_NOT_M1(nice(niceval));
	// bind the entire process to the core required
	// lets get our pid (no error for getpid)
	pid_t mypid=getpid();
	cpu_set_t myset;
	CPU_ZERO(&myset);
	CPU_SET(core, &myset);
	CHECK_NOT_M1(sched_setaffinity(mypid, sizeof(myset), &myset));
	float sum=0;
	for(unsigned int i=0; i<100000000; i++) {
		for(unsigned int j=0; j<10000000; j++) {
			sum+=j;
		}
		asm volatile ("" ::: "memory");
		*prog=*prog+1;
	}
	printf("child done with sum %f\n", sum);
}

int main(int argc, char** argv, char** envp) {
	if(argc<2) {
		fprintf(stderr, "%s: usage: %s [niceval core]...\n", argv[0], argv[0]);
		fprintf(stderr, "%s: forexample: %s 2 0 1 0\n", argv[0], argv[0]);
		fprintf(stderr, "%s: will run two processes nice values 2 and 1 on core 0\n", argv[0]);
		fprintf(stderr, "%s: or try: %s 0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0\n", argv[0], argv[0]);
		fprintf(stderr, "%s: to see the graph of cpu distribution\n", argv[0]);
		return EXIT_FAILURE;
	}
	const char* shm_name="shared_mem";
	// put this in shared memory
	// don't check the return address in case it's the first time we run
	//this software
	shm_unlink(shm_name);
	int smfd=CHECK_NOT_M1(shm_open(shm_name, O_CREAT|O_RDWR, 0));
	// we have to set the size otherwise it will not work
	CHECK_NOT_M1(ftruncate(smfd, getpagesize()));
	void* ptr=CHECK_NOT_VOIDP(mmap(
			NULL,	/* addr: dont recommend address */
			getpagesize(),	/* size: the size of the file */
			PROT_READ|PROT_WRITE,	/* prot: we just want read */
			MAP_SHARED,	/* flags: PRIVATE or SHARED ** MUST** be
					  specified */
			smfd,	/* fd: our file descriptor */
			0	/* offset: from the begining of the file */
			), MAP_FAILED);
	int* iptr=(int*)ptr;
	// lets fork...
	const int num_processes=(argc-1)/2;
	int process_num=0;
	for(int i=1; i<argc; i++) {
		int* prog=iptr+process_num;
		*prog=0;
		int niceval=atoi(argv[i]);
		i++;
		int cpu=atoi(argv[i]);
		make_child(niceval, prog, cpu);
		process_num++;
	}
	int loop=0;
	while(true) {
		printf("\riteration %d: progress: ", loop);
		for(int i=0; i<num_processes; i++) {
			// compiler barrier to make it read the values pointed
			//at by the pointers
			// again... This seems to work without the barrier but
			//still...
			asm volatile ("" ::: "memory");
			int val=*(iptr+i);
			if(i<num_processes-1) {
				printf("%d, ", val);
			} else {
				printf("%d", val);
			}
		}
		// printf("\r");
		fflush(stdout);
		sleep(1);
		loop++;
	}
	CHECK_NOT_M1(shm_unlink(shm_name));
	return EXIT_SUCCESS;
}
