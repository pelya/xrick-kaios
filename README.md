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

KaiOS 2.5.2 has Firefox/Gecko version 48. It does not support WebAssembly, but it supports asm.js.
You will need to add '-s WASM=0' to your Emscripten compiler flags.
It also does not support SharedArrayBuffer, so pthreads multithreading is not available.

Download and build Emscripten, as described on their page: https://emscripten.org/

Put directory with emsdk script into your $PATH

Launch build-debug.sh or build-release.sh

You should get compiled KaiOs package debug/application.zip or release/application.zip with manifest.webapp and Javascript code inside

Patch SDL2 to enable '*' and '#' keys:

Delete emsdk/upstream/emscripten/cache/sysroot/lib/wasm32-emscripten/libSDL2.a and emsdk/upstream/emscripten/cache/ports-builds/sdl2

Apply patch SDL_emscriptenevents.c.patch to SDL2 sources at emsdk/upstream/emscripten/cache/ports/sdl2

Launch build-debug.sh again, SDL2 should be recompiled automatically

To install application.zip to your device, launch command:

    git submodule update --init --recursive

Download and install ADB to path ~/bin/android-sdk/platform-tools/adb

If you already have Android SDK installed, you can symlink it to your ~/bin directory:

    ln -s /path/to/android-sdk ~/bin/android-sdk

On Debian, you can install adb from system packages:

    sudo apt-get install adb

Enable debug mode in your device by entering secret code on your home screen:

    *#*#33284#*#*

Plug in USB cable, run 'install.sh debug' or 'install.sh release'

You should see your app in the device app list, install script freezes at the end, you can kill it with Ctrl-C

KaiOS devices generate following SDL2 keycodes:

SDL_SCANCODE_BACKSPACE for 'Back' key

SDL_SCANCODE_KP_ENTER for 'Call' key

SDL_SCANCODE_ESCAPE for 'End Call' key, not present on all phones

SDL_SCANCODE_0 .. SDL_SCANCODE_9, SDL_SCANCODE_KP_HASH, SDL_SCANCODE_KP_MULTIPLY for numeric keyboard

SDL_SCANCODE_RETURN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN for directional pad

SDL_SCANCODE_AC_FORWARD and SDL_SCANCODE_AC_BACK for LSK and RSK (left soft key and right soft key)

SDL_SCANCODE_VOLUMEUP and SDL_SCANCODE_VOLUMEDOWN for volume keys

SDL_SCANCODE_HELP for Google Assistant button, it's not present on all phones, and it launches Google Assistant if you long-press it

By default all keypresses are passed to the OS, so for example holding D-Pad Center key for 2 seconds
will launch Google Assistant while your app is active, and End Call key will close your app immediately.
To prevent this, disable SDL_TEXTINPUT event after initializing video:

    SDL_EventState(SDL_TEXTINPUT, SDL_DISABLE);

If you are using landscape mode, the screen is rotated, but the D-Pad is not rotated,
and keycodes SDL_SCANCODE_LEFT/RIGHT/UP/DOWN are sent for the wrong direction keys.
To fix this, add '-D KAIOS_SWAP_NAVIGATION_KEYS=1' to your compiler flags, and include sys_kaios.h everywhere,
it will redefine SDL keycodes for navigation keys for the D-Pad rotated to the side.

With Emscripten, your main loop should not block anywhere - do not call sleep() or SDL_Delay() or SDL_WaitEvent().
Use emscripten_set_main_loop() to call your main loop function after you initialized video and everything else.

Alternatively, you can add parameter '-s ASYNCIFY=1' to your compiler flags, this will make your code
unwind and rewind the stack whenever it enters sleep or screen update function, and process web browser main loop,
this makes your app run significantly slower, but you don't need to rewrite your code at all.

Use sys_exit_app() to close the app.
if you simply call exit(0) the app won't clear it's state and will show black screen on the next launch.

After calling SDL_CreateWindow(), call sys_hide_splash_image() to hide splash image.
If you don't call it, the splash image is hidden automatically in 10 seconds.

To write data to files that will not be deleted after you close the app, you have to mount a writable file system,
and sync it after writing to file.
Files should be saved to directory defined in FS_WRITE_MOUNT_POINT.
First call sys_fs_init() from your init code.
Then check for sys_fs_init_get_done() to return 1 in a loop, before reading or writing any files inside FS_WRITE_MOUNT_POINT.
Call sys_fs_sync() after writing any files, to push data to filesystem database.
Then check for sys_fs_sync_get_done() to return 1 when FS sync is finished.

To prevent the screen from sleeping, call sys_take_wake_lock(), and call sys_free_wake_lock() to enable sleep.

The app must mute any music or audio when the phone lid is closed or when the screen is dimmed.
The app will receive SDL_WINDOWEVENT notifications from SDL. Whenever the app receives events

    if (event.type == SDL_WINDOWEVENT && (
        event.window.event == SDL_WINDOWEVENT_FOCUS_LOST ||
        event.window.event == SDL_WINDOWEVENT_HIDDEN))

it must mute the audio using SDL_PauseAudio(1) call.
The app can unmute the audio only after receiving both events
SDL_WINDOWEVENT_SHOWN and SDL_WINDOWEVENT_FOCUS_GAINED.

To debug your code on the device, compile the app using the script build-debug.sh,
then you can use printf() in the code to write debug messages to the text area on the screen

KaiStore manually tests each app submission, and checks that it does not crash on phones with 256 Mb RAM.
You can monitor your app memory usage by using 'top' command and watching RSS memory usage:

    adb shell top -m 5 -s rss

QA testing may take from 2 days up to 3 weeks, if there are many other apps to test.

To detect the amount of RAM on the device - add following code to your manifest.webapp:

    "type": "privileged",
    "permissions": {
        "feature-detection": {}
    },
    "dependencies": {
        "ads-sdk": "1.4.1"
    }

Then you can use the function sys_get_device_ram_size_megabytes() to get RAM size in megabytes.
KaiOS phones have either 256 or 512 MB RAM. Note that the amount of memory available to Javascript
will be much smaller than the physical RAM size, around 200 MB for devices with 512 MB RAM.

Maximum size of application.zip for uploading to KaiOS Store is currently 20 Mb, and QA will complain if it's bigger than 6 Mb.

The store accepts games in landscape mode. Device screen is 320x240 pixels, and SDL will stretch smaller video output to fullscreen.

KaiOS Store does provide app updates, however it would require user interaction, so it's discouraged.

The KaiAds is deeply integrated to the KaiStore, the app analytics is part of the KaiAds SDK
and developers need to go to KaiAds page to access the app install and usage statistics.

If apps aren't monetized, KaiStore team would mark it a low priority and the QA might be delayed.

To show a fullscreen advertisement, call sys_show_fullscreen_advertisement(),
it should be accessible from somewhere in the app, like settings dialog.

You will need to modify publisher ID and app name in app/sys.js in sys_fetch_new_advertisement().
App name should also be changed in app/index.html.

There is supposed to be a hidden API to use UDP sockets directly on KaiOS, without WebRTC wrappers:

https://www.w3.org/TR/tcp-udp-sockets/

https://developer.mozilla.org/en-US/docs/Archive/B2G_OS/API/UDPSocket

It requires privileged app permissions - add following code to your manifest.webapp:

    "type", "privileged",
    "dependencies": {
        "ads-sdk": "1.4.1"
    }

Privileged app cannot contain Javascript code embedded into HTML directly,
you must use <script src="..."> everywhere, the embedded Javascript won't be executed.

It is possible to read files from SD card without "device-storage:sdcard" permission,
by registering an activity to accept the file from the file manager or from Downloads app:

https://github.com/pelya/doom-kaios/blob/master/app/manifest.webapp#L36

https://github.com/pelya/doom-kaios/blob/master/app/sys.js#L125
