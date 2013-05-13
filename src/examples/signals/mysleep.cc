/*
 * This file is part of the linuxapi project.
 * Copyright (C) 2011-2013 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * The linuxapi package is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * The linuxapi package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the GNU C Library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA.
 */

#include <firstinclude.h>
#include <signal.h>	// for SIGALRM
#include <stdio.h>	// for printf(3), fprintf(3)
#include <unistd.h>	// for alarm(2), unistd(2)
#include <stdlib.h>	// for EXIT_SUCCESS, atoi(3)
#include <us_helper.h>	// for CHECK_ZERO(), register_handler_sigaction()

/*
 * This is an example of how to implement the sleep function using
 * alarm(2) and pause(2)
 */

static void handler(int sig, siginfo_t *si, void *unused) {
	// do nothing?!?
}

void my_sleep(int seconds) {
	CHECK_ZERO(alarm(seconds));
	int ret=pause();
	CHECK_ASSERT(ret==-1 && errno==EINTR);
}

int main(int argc, char** argv, char** envp) {
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [seconds]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	// parameters
	int seconds=atoi(argv[1]);
	// register the signal handler for SIGALRM
	// we have to do this since the default handler for SIGALRM
	// will terminate the running program
	register_handler_sigaction(SIGALRM, handler);
	// demonstrate the use of our sleep function
	while(true) {
		printf("before sleep\n");
		my_sleep(seconds);
		printf("woke up\n");
	}
	return EXIT_SUCCESS;
}
