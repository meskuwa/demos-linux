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

#ifndef __trace_utils_h
#define __trace_utils_h

/*
 * This is a collection of helper function to help with tracing
 */

/* THIS IS A C FILE, NO C++ here */

#include <firstinclude.h>
#include <pthread_utils.h>	// for gettid()

static inline void debug(bool short_print, const char *file, const char *function, int line, const char *fmt, ...) {
	extern char *program_invocation_short_name;
	const int BUFSIZE=1024;
	char str[BUFSIZE];
	va_list args;
	pid_t pid=getpid();
	// pid_t tid=gettid_cached();
	pid_t tid=gettid();
	if(short_print) {
		snprintf(str, BUFSIZE, "%s\n", fmt);
	} else {
		snprintf(str, BUFSIZE, "%s %d/%d %s %s %d: %s\n", program_invocation_short_name, pid, tid, file, function, line, fmt);
	}
	va_start(args, fmt);
	vfprintf(stderr, str, args);
	va_end(args);
}

void debug(bool short_print, const char *file, const char *function, int line, const char *fmt, ...) __attribute__((format(printf, 5, 6)));

/*
 * Semantics of these macros:
 * TRACE - always enabled and always shows max info (usually for debug)
 * DEBUG - cancelled by default and shows max info (turn it on with DO_DEBUG).
 * INFO, WARNING, ERROR, FATAL - doesn't show a lot of info (just the message).
 */
#define TRACE(fmt, args ...) debug(false, __FILE__, __FUNCTION__, __LINE__, fmt, ## args)
#ifdef DO_DEBUG
#define DEBUG(fmt, args ...) debug(false, __FILE__, __FUNCTION__, __LINE__, fmt, ## args)
#else
#define DEBUG(fmt, args ...) do {} while(0)
#endif
#define INFO(fmt, args ...) debug(true, __FILE__, __FUNCTION__, __LINE__, fmt, ## args)
#define WARNING(fmt, args ...) debug(true, __FILE__, __FUNCTION__, __LINE__, fmt, ## args)
#define ERROR(fmt, args ...) debug(true, __FILE__, __FUNCTION__, __LINE__, fmt, ## args)
#define FATAL(fmt, args ...) debug(true, __FILE__, __FUNCTION__, __LINE__, fmt, ## args)

#endif	/* !__trace_utils_h */
