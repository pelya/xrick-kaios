=== About ===

Remember Rick Dangerous?

Way before Lara Croft, back in the 1980's and early 1990's, Rick Dangerous was the Indiana Jones of computer games,
running away from rolling rocks, avoiding traps, from South America to a futuristic missile base via Egypt and the
Schwarzendumpf castle.

**xrick** is a clone of Rick Dangerous, produced by carefully reverse-engineering the PC and Atari versions of the
game, and re-coding in C. It has been ported to Windows, Linux, but also BeOs, Amiga, QNX, and all sorts
of gaming console.

You can read more about Rick Dangerous straight from his creator, [Simon Phipps](https://www.simonphipps.com/games/rickdangerous/),
and more about xrick at the original [xrick page](http://www.bigorno.net/xrick). The code for xrick was only available 
as Zip files on that page: the goal of this repository is to release it in a more convenient way.

So far, it contains:
* The "Dec 12th, 2002" release (#021212) which is the last version I published in 2002
* The "May, 2005" release (#050500) which was never released, and is a bit cleaner (?)
* Ported from SDL to SDL2
* With adjustments so it can build with [emscripten](https://emscripten.org/)

This is all work-in-progress and will be updated.

=== KaiOS instructions ===

Download and build Emscripten, as described on their page: https://emscripten.org/

Put directory with emsdk script into your $PATH

Launch build.sh

You should get compiled KaiOs package out/application.zip with manifest.webapp and Javascript code inside

Patch SDL2 to enable '*' and '#' keys:

Delete ~/.emscripten_cache/wasm-obj/libSDL2.a and ~/.emscripten_cache/wasm-obj/ports-builds/sdl2

Apply patch build/app/emscripten-kaios-keycodes.patch to SDL2 sources at ~/.emscripten_ports/sdl2

Launch build.sh again, SDL2 should be recompiled automatically

To install application.zip to your device, launch command:

git submodule update --init --recursive

Follow instructions in build/make-kaios-install/README.md to download xulrunner with xpcshell tool, and adb tool.

On Debian, you can install adb from system packages:

sudo apt-get install adb

Enable debug mode in your device by entering secret code on your home screen:

    *#*#33284#*#*

Plug in USB cable, run install.sh

You should see your app in the device app list, install script freezes at the end, you can kill it with Ctrl-C

KaiOS devices generate following SDL2 keycodes:

SDL_SCANCODE_BACKSPACE for 'Back' key, not present on all phones

SDL_SCANCODE_0 .. SDL_SCANCODE_9, SDL_SCANCODE_KP_HASH, SDL_SCANCODE_KP_MULTIPLY for numeric keyboard

SDL_SCANCODE_RETURN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN for directional pad

SDL_SCANCODE_AC_FORWARD and SDL_SCANCODE_AC_BACK for LSK and RSK (left soft key and right soft key)

With Emscripten, your main loop should not block anywhere - do not call sleep() or SDL_Delay() or SDL_WaitEvent()

Use emscripten_set_main_loop() to call your main loop function after you initialized video and everything else

Use emscripten_cancel_main_loop() and EM_ASM( window.open('','_self').close(); ); to exit the app,
if you simply call exit(0) the app won't clear it's state and will show black screen on the next launch

To write data to files that will not be deleted after you close the app, you have to mount a writable file system,
and sync it after writing to file, check sys_fs_init() and sys_fs_sync() in the code

To debug your code on the device, compile the app like this:

env DEBUG=1 ./build.sh

then you can use printf() in the code to write debug messages to text area on screen
