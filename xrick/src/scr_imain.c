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

#include "draw.h"
#include "pics.h"
#include "control.h"
#include "fb.h"

#include "tiles.h"

#ifdef EMSCRIPTEN
#include "emscripten.h"
#endif

#define IMAIN_PERIOD 50

static void konami_code(void);

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
	U8 i, s[32];

	konami_code();

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

struct controls_t {
	unsigned control_status:5;
	unsigned key_bullet:1;
	unsigned key_bomb:1;
	unsigned key_stick:1;
};

static int controls_equal(struct controls_t * c1, struct controls_t * c2)
{
	return
		c1->control_status == c2->control_status &&
		c1->key_bullet == c2->key_bullet &&
		c1->key_bomb == c2->key_bomb &&
		c1->key_stick == c2->key_stick;
}

static struct controls_t konami_seq[] = {
	{CONTROL_UP, 0, 0, 0},
	{0, 0, 0, 0},
	{CONTROL_UP, 0, 0, 0},
	{0, 0, 0, 0},
	{CONTROL_DOWN, 0, 0, 0},
	{0, 0, 0, 0},
	{CONTROL_DOWN, 0, 0, 0},
	{0, 0, 0, 0},
	{CONTROL_LEFT, 0, 0, 0},
	{0, 0, 0, 0},
	{CONTROL_RIGHT, 0, 0, 0},
	{0, 0, 0, 0},
	{CONTROL_LEFT, 0, 0, 0},
	{0, 0, 0, 0},
	{CONTROL_RIGHT, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 0, 1, 0},
	{0, 0, 0, 0},
	{0, 1, 0, 0},
};

static int konami_counter = 0;

void konami_code(void)
{
	struct controls_t controls = {control_status, KEY_BULLET, KEY_BOMB, KEY_STICK};

	if (konami_counter > 0 && controls_equal(&konami_seq[konami_counter - 1], &controls))
		return; // Previous key still pressed

	if (controls_equal(&konami_seq[konami_counter], &controls))
		konami_counter++;
	else
		konami_counter = 0;

	printf("konami_counter %d\n", konami_counter);

	if (konami_counter >= sizeof(konami_seq) / sizeof(konami_seq[0]))
	{
		// What do we give to a user entering cheat code in a game with no ads and unlimited continues?
		// A full-screen advertisement!
		//printf("\n\n=== Konami code! ===\n\n\n");
#ifdef EMSCRIPTEN
		EM_ASM( if (lastKaiAd !== false) lastKaiAd.call('display'); );
#endif
	}
}

/* eof */


