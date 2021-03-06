/*
 * xrick/include/control.h
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

#ifndef _CONTROL_H
#define _CONTROL_H

#include "system.h"
#include <SDL_keyboard.h>

#define CONTROL_UP 0x08
#define CONTROL_DOWN 0x04
#define CONTROL_LEFT 0x02
#define CONTROL_RIGHT 0x01
#define CONTROL_PAUSE 0x80
#define CONTROL_END 0x40
#define CONTROL_EXIT 0x20
#define CONTROL_FIRE 0x10

#ifdef EMSCRIPTEN
#define KEY_BULLET		SDL_GetKeyboardState(NULL)[SDL_SCANCODE_8]
#define KEY_BOMB		SDL_GetKeyboardState(NULL)[SDL_SCANCODE_KP_HASH]
#define KEY_STICK		SDL_GetKeyboardState(NULL)[SDL_SCANCODE_9]
#else
#define KEY_BULLET		SDL_GetKeyboardState(NULL)[SDL_SCANCODE_Z]
#define KEY_BOMB		SDL_GetKeyboardState(NULL)[SDL_SCANCODE_X]
#define KEY_STICK		SDL_GetKeyboardState(NULL)[SDL_SCANCODE_C]
#endif

extern U8 control_status;
extern U8 control_last;
extern U8 control_active;

#endif

/* eof */


