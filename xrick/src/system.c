/*
 * xrick/src/system.c
 *
 * Copyright (C) 1998-2019 bigorno (bigorno@bigorno.net). All rights reserved.
 *
 * The use and distribution terms for this software are contained in the file
 * named README, which can be found in the root of this distribution. By
 * using this software in any fashion, you are agreeing to be bound by the
 * terms of this license.
 *
 * You must not remove this notice, or any other, from this software.
 */

#include <SDL.h>

#include <stdarg.h>   /* args for sys_panic */
#include <fcntl.h>    /* fcntl in sys_panic */
#include <stdio.h>    /* printf */
#include <stdlib.h>

#ifdef EMSCRIPTEN
#include "emscripten.h"
#endif

#include "system.h"

/*
 * Panic
 */
void
sys_panic(char *err, ...)
{
	va_list argptr;
	char s[1024];

	/* FIXME what is this? */
	/* change stdin to non blocking */
	/*fcntl(0, F_SETFL, fcntl (0, F_GETFL, 0) & ~FNDELAY);*/
	/* NOTE HPUX: use ... is it OK on Linux ? */
	/* fcntl(0, F_SETFL, fcntl (0, F_GETFL, 0) & ~O_NDELAY); */

	/* prepare message */
	va_start(argptr, err);
	vsprintf(s, err, argptr);
	va_end(argptr);

	/* print message and die */
	printf("%s\npanic!\n", s);
	exit(1);
}


/*
 * Print a message
 */
void
sys_printf(char *msg, ...)
{
#ifdef ENABLE_LOG
	va_list argptr;
	char s[1024];

	/* FIXME what is this? */
	/* change stdin to non blocking */
	/* fcntl(0, F_SETFL, fcntl (0, F_GETFL, 0) & ~FNDELAY); */
	/* NOTE HPUX: use ... is it OK on Linux ? */
	/* fcntl(0, F_SETFL, fcntl (0, F_GETFL, 0) & ~O_NDELAY); */

	/* prepare message */
	va_start(argptr, msg);
	vsnprintf(s, sizeof(s), msg, argptr);
	va_end(argptr);
	printf("%s", s);
#endif
}

/*
 * Return number of milliseconds elapsed since first call
 */
U32
sys_gettime(void)
{
	static U32 ticks_base = 0;
	U32 ticks;

	ticks = SDL_GetTicks();

	if (!ticks_base)
		ticks_base = ticks;

	return ticks - ticks_base;
}

/*
 * Sleep a number of milliseconds
 */
void
sys_sleep(int s)
{
	SDL_Delay(s);
}

static int fs_initialized = 0;

void sys_fs_init(void)
{
	if (!fs_initialized) {
		fs_initialized = 1;
#ifdef EMSCRIPTEN
		EM_ASM({
			// Make a directory other than '/'
			FS.mkdir(UTF8ToString($0));
			// Then mount with IDBFS type
			FS.mount(IDBFS, {}, UTF8ToString($0));
			// Then sync
			// var sys_fs_init_is_done = 0; // Defined in HTML file
			FS.syncfs(true, function (err) {
				if (err) alert('Error initializing filesystem');
				//Module.print('sys_fs_init done, mount point ' + UTF8ToString($0));
				sys_fs_init_is_done = 1;
			});
		}, FS_WRITE_MOUNT_POINT);
#endif // EMSCRIPTEN
	}
}

int sys_fs_init_is_done(void)
{
	if (!fs_initialized) {
		sys_printf("Please call sys_fs_init() first\n");
		printf("Please call sys_fs_init() first\n");
		exit(1);
	}

	int status = 1;
#ifdef EMSCRIPTEN
	status = EM_ASM_INT({ return sys_fs_init_is_done; });
#endif // EMSCRIPTEN
	return status;
}

void sys_fs_sync(void)
{
	if (!fs_initialized) {
		sys_printf("Please call sys_fs_init() first\n");
		printf("Please call sys_fs_init() first\n");
		exit(1);
	}
#ifdef EMSCRIPTEN
	EM_ASM(
		FS.syncfs(false, function (err) {
			if (err) alert('Error writing to filesystem');
			//Module.print('sys_fs_sync done');
		});
	);
#endif // EMSCRIPTEN
}

/* eof */
