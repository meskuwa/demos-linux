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
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS, malloc(3)
#include <us_helper.h>	// for CHECK_ASSERT()

/*
 * This example shows that the compiler aligns fields to 4/8
 * bytes according to whether you are on a on a 32/64 bit CPU.
 * It also shows that malloc always returns aligned memory.
 * It also shows that parameters to functions are 4/8 aligned.
 *
 * The result is that all of your fields are aligned on a 4/8
 * alignment which results in better performance.
 *
 * References:
 * http://www.geeksforgeeks.org/structure-member-alignment-padding-and-data-packing/
 * http://en.wikipedia.org/wiki/Data_structure_alignment
 */

struct small {
	char c1;
	char c2;
};

struct medium {
	char c1;
	int i1;
	char c2;
};

struct medium2 {
	char c1;
	char c2;
	int i1;
};

void myfunction(char c1, int i1, char c2) {
	CHECK_ASSERT((int)&c1%4==0);
	CHECK_ASSERT((int)&i1%4==0);
	CHECK_ASSERT((int)&c2%4==0);
}

int main(int argc, char** argv, char** envp) {
	printf("sizeof(small)=%d\n", sizeof(small));
	printf("sizeof(medium)=%d\n", sizeof(medium));
	printf("sizeof(medium2)=%d\n", sizeof(medium2));
	for(int i=1;i<200;i++) {
		char* p=(char*)malloc(i);
		CHECK_ASSERT(p!=NULL);
		CHECK_ASSERT((int)p%4==0);
	}
	return EXIT_SUCCESS;
}