#!/bin/bash
# xrick emscripten build script
# runs in build.tmp

# configure paths
[ -z "$PATH_PYTHON" ] && PATH_PYTHON="which python | xargs dirname"
[ -z "$PATH_EMSDK" ] && PATH_EMSDK="`which emsdk | xargs dirname`"
[ -z "$PATH_XRICK" ] && PATH_XRICK="`realpath $0 | xargs dirname`"
[ -z "$PATH_OUTPUT" ] && PATH_OUTPUT="$PATH_XRICK/out"

# ensure Python
echo Adding Python directory to PATH:
echo PATH += $PATH_PYTHON
PATH=$PATH_PYTHON:$PATH
echo

# configure emsdk
echo Configuring EmSdk...
source "$PATH_EMSDK/emsdk_env.sh"

SRC="$PATH_XRICK/xrick/src"
INC="$PATH_XRICK/xrick/include"
DATA="$PATH_XRICK/data"

echo Compiling...

rm -rf "$PATH_OUTPUT"
mkdir -p "$PATH_OUTPUT"
cd "$PATH_OUTPUT"

HTML_TEMPLATE="$PATH_XRICK/build/app/shell_minimal.html"
[ -n "$DEBUG" ] && HTML_TEMPLATE="$PATH_XRICK/build/app/shell_minimal_debug.html"

# WebAssembly not supported on KaiOS
emcc $SRC/*.c -o xrick.html \
    -I "$INC" \
    -s USE_SDL=2 \
    -s FORCE_FILESYSTEM=1 \
    -s WASM=0 \
    -D NOZLIB \
    -l idbfs.js \
    -O3 \
    --preload-file "$DATA@/data.zip" \
    --shell-file "$HTML_TEMPLATE" \
    || exit 1

echo Copy files...
cp "$PATH_XRICK/build/app/manifest.webapp" ./
cp "$PATH_XRICK/build/app/icon_56.png" ./
cp "$PATH_XRICK/build/app/icon_112.png" ./
cp "$PATH_XRICK/build/app/icon_bg_56.png" ./
cp "$PATH_XRICK/build/app/icon_bg_112.png" ./
cp "$PATH_XRICK/build/app/img_xrick.jpg" ./
cp "$PATH_XRICK/build/app/splash.png" ./
cp "$PATH_XRICK/build/app/favicon.ico" ./
cp "$PATH_XRICK/build/kaiads/kaiads.v3.min.js" ./

rm -f application.zip
zip application.zip * --exclude application.zip

echo Done.
