/*
 * xrick/src/scr_gameover.c
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

#include "stddef.h" /* NULL */

#include "system.h"
#include "game.h"
#include "screens.h"
#include "sysvid.h"
#include "sounds.h"

#include "draw.h"
#include "control.h"
#include "tiles.h"
#include "fb.h"

/*
 * Display the game over screen
 *
 * return: SCREEN_RUNNING, SCREEN_DONE, SCREEN_EXIT
 */
U8
screen_gameover(void)
{
	static U8 seq = 0;
	static U8 period = 0;
#ifdef GFXST
	static U32 tm = 0;
#endif
#ifdef ENABLE_SOUND
	static sound_t *snd;
	static U8 chan;
#endif

	if (seq == 0) {
		tiles_setBank(0);
		seq = 1;
		period = game_period; /* save period, */
		game_period = 50;     /* and use our own */
		sysvid_setGamma(255);
#ifdef ENABLE_SOUND
		sounds_setMusic("sounds/gameover.wav", 1);
#endif
	}

	switch (seq) {
	case 1:  /* display banner */
#ifdef GFXST
		fb_clear();
		tm = sys_gettime();
#endif
#ifdef GFXPC
		tiles_setFilter(0xaaaa);
#endif
		tiles_paintListAt(screen_gameovertxt, 120 - 0x20, 80);

		game_rects = &draw_SCREENRECT;
		//seq = 2;
		//break; // FALLTHROUGH

	case 2:  /* wait for key pressed */
		if ((control_status & CONTROL_FIRE) || KEY_BULLET)
			seq = 3;
#ifdef GFXST
		else if (sys_gettime() - tm > SCREEN_TIMEOUT)
			seq = 4;
#endif
		//else
		//	sys_sleep(50);
		break;

	case 3:  /* wait for key released */
		if (!((control_status & CONTROL_FIRE) || KEY_BULLET))
			seq = 4;
		//else
		//	sys_sleep(50);
		break;
	}

	if (control_status & CONTROL_EXIT)  /* check for exit request */
		return SCREEN_EXIT;

	if (seq == 4) {  /* we're done */
		fb_clear();
		seq = 0;
		game_period = period;
		return SCREEN_DONE;
	}

  return SCREEN_RUNNING;
}

/* eof */

