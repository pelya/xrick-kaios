/*
 * xrick/src/scr_imain.c
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

#include <stdio.h>  /* sprintf */

#include "system.h"
#include "game.h"
#include "screens.h"
#include "sysvid.h"
#include "sounds.h"
#include "env.h"

#include "draw.h"
#include "pics.h"
#include "control.h"
#include "fb.h"

#include "tiles.h"

#ifdef EMSCRIPTEN
#include "emscripten.h"
#endif

#define IMAIN_PERIOD 50;

#define FS_OPTIONS FS_WRITE_MOUNT_POINT "/options.cfg"

void
load_options(void)
{
	FILE *ff = fopen(FS_OPTIONS, "rb");
	if (!ff)
	{
		return;
	}
	char buf[16] = "0";
	fread(buf, 1, 1, ff);
	fclose(ff);
	if (buf[0] == '0' && syssnd_getMute())
		syssnd_toggleMute();
	if (buf[0] == '1' && !syssnd_getMute())
		syssnd_toggleMute();
}

static void
save_options(void)
{
	FILE *ff = fopen(FS_OPTIONS, "wb");
	if (ff)
	{
		fprintf(ff, "%d", syssnd_getMute());
		fclose(ff);
		sys_fs_sync();
	}
}

static U8
screen_options(void);

/*
 * Main introduction
 *
 * return: SCREEN_RUNNING, SCREEN_DONE, SCREEN_EXIT
 */
U8
screen_introMain(void)
{
	static U8 seq = 0;
	static U8 seen = 0;
	static U8 first = TRUE;
	static U8 period = 0;
	static U32 tm = 0;
	static U8 options = FALSE;
	U8 i, s[32];

	if (seq == 0)
	{
		tiles_setBank(0);
		if (first == TRUE)
			seq = 1;
		else
			seq = 10;
		period = game_period;
		game_period = IMAIN_PERIOD;
		game_rects = &draw_SCREENRECT;
#ifdef ENABLE_SOUND
		sounds_setMusic("sounds/tune5.wav", -1);
#endif
	}

	if (options)
	{
		if (screen_options() == SCREEN_DONE)
		{
			options = FALSE;
			seq = 1;
		}
		return SCREEN_RUNNING;
	}

	switch (seq)
	{
		case 1:  /* dispay hall of fame */
			fb_clear();
			sysvid_setGamma(0);
			tm = sys_gettime();

#ifdef GFXPC
			/* Rick Dangerous title */
			tiles_setFilter(0xaaaa);
			tiles_paintList(screen_imainrdt, fb_at(32, 16));

			/* Core Design copyright + press space to start */
			tiles_setFilter(0x5555);
			tiles_paintList(screen_imaincdc, fb_at(64, 80));
#endif

#ifdef GFXST
			img_paintPic(-0x20, 0, 0x140, 0xc8, pic_splash);
#endif

			tiles_paintListAt(".........@@" TILES_NULLCHAR, 176 - 8, 170);
			tiles_paintListAt(".........@@" TILES_NULLCHAR, 176 - 8, 184);
			tiles_paintListAt("..........." TILES_NULLCHAR, 176 - 8, 177);
			tiles_paintListAt( "OPTIONS"    TILES_NULLCHAR, 183 - 8, 180);

			game_period = period/2;
			seq = 2;
			break;

		case 2: /* fade-in */
			if (fb_fadeIn())
			{
				game_period = IMAIN_PERIOD;
				seq = 3;
			}
			break;

		case 3:  /* wait for key pressed or timeout */
			if ((control_status & CONTROL_FIRE) || KEY_BULLET)
				seq = 4;
			else if (sys_gettime() - tm > SCREEN_TIMEOUT)
			{
				seen++;
				game_period = period/2;
				seq = 8;
			}
			if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_AC_FORWARD] || SDL_GetKeyboardState(NULL)[SDL_SCANCODE_F12])
			{
				options = TRUE;
			}
			break;

		case 4:  /* wait for key released */
			if (!((control_status & CONTROL_FIRE) || KEY_BULLET))
			{
				if (seen++ == 0)
					seq = 8;
				else
				{
					game_period = period/2;
					seq = 28;
				}
			}
			break;

		case 8: /* fade-out */
			if (fb_fadeOut())
			{
				game_period = IMAIN_PERIOD;
				seq = 10;
			}
			break;

		case 10:  /* display Rick Dangerous title and Core Design copyright */
			fb_clear();
			tm = sys_gettime();

			/* hall of fame title */
#ifdef GFXPC
			tiles_setFilter(0xaaaa);
			tiles_paintListAt(screen_imainhoft, 32, 0);
#endif
#ifdef GFXST
			img_paintPic(-0x20, 0, 0x140, 0x20, pic_haf);
#endif

			/* hall of fame content */
#ifdef GFXPC
			tiles_setFilter(0x5555);
#endif
			for (i = 0; i < 8; i++)
			{
				sprintf((char *)s, "%06d@@@....@@@%s",
					game_hscores[i].score, game_hscores[i].name);
				s[26] = TILES_NULL;
				tiles_paintListAt(s, 56 - 0x20, 40 + i*2*8);
			}

			tiles_paintListAt(".........@@" TILES_NULLCHAR, 176 - 8, 170);
			tiles_paintListAt(".........@@" TILES_NULLCHAR, 176 - 8, 184);
			tiles_paintListAt("..........." TILES_NULLCHAR, 176 - 8, 177);
			tiles_paintListAt( "OPTIONS"    TILES_NULLCHAR, 183 - 8, 180);

			game_period = period/2;
			seq = 11;
			break;

		case 11: /* fade-in */
			if (fb_fadeIn())
			{
				game_period = IMAIN_PERIOD;
				seq = 12;
			}
			break;

		case 12:  /* wait for key pressed or timeout */
			if ((control_status & CONTROL_FIRE) || KEY_BULLET)
				seq = 13;
			else if (sys_gettime() - tm > SCREEN_TIMEOUT)
			{
				seen++;
				seq = 18;
			}
			if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_AC_FORWARD] || SDL_GetKeyboardState(NULL)[SDL_SCANCODE_F12])
			{
				options = TRUE;
			}
			break;

		case 13:  /* wait for key released */
			if (!((control_status & CONTROL_FIRE) || KEY_BULLET))
			{
				if (seen++ == 0)
					seq = 18;
				else
				{
					game_period = period/2;
					seq = 28;
				}
			}
			break;

		case 18: /* fade-out */
			if (fb_fadeOut())
			{
				game_period = IMAIN_PERIOD;
				seq = 1;
			}
			break;

		case 28: /* fade-out */
			if (fb_fadeOut())
			{
				game_period = IMAIN_PERIOD;
				seq = 30;
			}
			break;

	}

	if (control_status & CONTROL_EXIT)  /* check for exit request */
		return SCREEN_EXIT;

	if (seq == 30) /* we're done */
	{
		fb_clear();
		seq = 0;
		seen = 0;
		first = FALSE;
		game_period = period;
		sysvid_setGamma(255);
		return SCREEN_DONE;
	}
	else
		return SCREEN_RUNNING;
}

U8
screen_options(void)
{
	static S8 pos = 0;
	enum { POS_LAST = 2 };
	static U8 pressed = 0;

	fb_clear();

	tiles_paintListAt("........."   TILES_NULLCHAR, 76 + 28, 10);
	tiles_paintListAt("........."   TILES_NULLCHAR, 76 + 28, 24);
	tiles_paintListAt("........."   TILES_NULLCHAR, 76 + 28, 17);
	tiles_paintListAt( "OPTIONS"    TILES_NULLCHAR, 83 + 28, 20);

	tiles_paintListAt("................"  TILES_NULLCHAR, 76, 70 + pos * 20);
	tiles_paintListAt("................"  TILES_NULLCHAR, 76, 84 + pos * 20);
	tiles_paintListAt("................"  TILES_NULLCHAR, 76, 77 + pos * 20);

	if (syssnd_getMute())
		tiles_paintListAt(
					   "SOUND@@@@@@OFF"   TILES_NULLCHAR, 83, 80);
	else
		tiles_paintListAt(
					   "SOUND@@@@@@@ON"   TILES_NULLCHAR, 83, 80 + 20 * 0);

	tiles_paintListAt( "RESET@PROGRESS"   TILES_NULLCHAR, 83, 80 + 20 * 1);

	tiles_paintListAt( "DONE@@@@@@@@@@"   TILES_NULLCHAR, 83, 80 + 20 * 2);

	if (control_status & CONTROL_EXIT)
	{
		pos = 0;
		save_options();
		return SCREEN_DONE;
	}

	if (control_status & CONTROL_UP)
	{
		pressed = CONTROL_UP;
		return SCREEN_RUNNING;
	}
	else if (pressed == CONTROL_UP)
	{
		pressed = 0;
		pos --;
		if (pos < 0)
			pos = POS_LAST;
	}

	if (control_status & CONTROL_DOWN)
	{
		pressed = CONTROL_DOWN;
		return SCREEN_RUNNING;
	}
	else if (pressed == CONTROL_DOWN)
	{
		pressed = 0;
		pos ++;
		if (pos > POS_LAST)
			pos = 0;
	}

	if ((control_status & CONTROL_FIRE) || KEY_BULLET)
	{
		pressed = CONTROL_FIRE;
		return SCREEN_RUNNING;
	}
	else if (pressed == CONTROL_FIRE)
	{
		pressed = 0;
		if (pos == 0)
		{
			syssnd_toggleMute();
		}
		if (pos == 1)
		{
			env_map = 0;
			env_submap = 0;
			map_saveProgress();
		}
		if (pos == 1 || pos == 2)
		{
			pos = 0;
			save_options();
#ifdef EMSCRIPTEN
			EM_ASM( if (lastKaiAd !== false) lastKaiAd.call('display'); );
#endif
			return SCREEN_DONE;
		}
	}

	return SCREEN_RUNNING;
}

/* eof */


