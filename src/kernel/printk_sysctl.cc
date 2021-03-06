/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2016 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * linuxapi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * linuxapi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linuxapi. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * This example shows how to control kernel logging level using a user space program.
 * You can also use /proc or sysctl via the command line to do the same thing
 */

#include <firstinclude.h>
#include <stdio.h>	// for printf(3), fprintf(3), stderr
#include <sys/sysctl.h>	// for sysctl(3)
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <us_helper.h>	// for ARRAY_SIZEOF()

int name[]={CTL_KERN, KERN_PRINTK};
int printk_params[4];
int new_params[4];

int main(int argc, char** argv, char** envp) {
	size_t paramlth=sizeof(printk_params);
	if(argc==1) {
		/* report */
		CHECK_NOT_M1(sysctl(name, ARRAY_SIZEOF(name), printk_params, &paramlth, 0, 0));
		printf("got %zd bytes:\n", paramlth);
		printf("console_loglevel: %d\n", printk_params[0]);
		printf("default_message_loglevel: %d\n", printk_params[1]);
		printf("minimum_console_loglevel: %d\n", printk_params[2]);
		printf("default_console_loglevel: %d\n", printk_params[3]);
	} else if(argc==5) {
		for(int i=0; i<4; i++)
			new_params[i]=atoi(argv[i+1]);
		/* set */
		CHECK_NOT_M1(sysctl(name, ARRAY_SIZEOF(name), 0, 0, new_params, sizeof(new_params)));
		printf("set new printk parameters\n");
	} else {
		fprintf(stderr, "Call: %s [N N N N]\n", argv[0]);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
