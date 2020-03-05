#!/bin/bash
# xrick emscripten build script
# runs in build.tmp

# configure pathsg
[ -z "$PATH_PYTHON" ] && PATH_PYTHON="which python | xargs dirname"
[ -z "$PATH_EMSDK" ] && PATH_EMSDK="`which emsdk | xargs dirname`"
[ -z "$PATH_XRICK" ] && PATH_XRICK="`realpath $0 | xargs dirname`"

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

# WebAssembly not supported on KaiOS
emcc $SRC/*.c -o xrick.html \
    -I "$INC" \
    -s USE_SDL=2 \
    -s FORCE_FILESYSTEM=1 \
    -s WASM=0 \
    -D NOZLIB \
    -O2 \
    --preload-file "$DATA@/data.zip" \
    --shell-file $PATH_XRICK/build/app/shell_minimal.html \
    || exit 1

echo

echo Copy files...
cp "$PATH_XRICK/build/app/manifest.webapp" ./
cp "$PATH_XRICK/build/app/icon_56.png" ./
cp "$PATH_XRICK/build/app/icon_112.png" ./
cp "$PATH_XRICK/build/app/favicon.ico" ./

rm -f application.zip
zip application.zip manifest.webapp *.data *.js *.html *.html.mem *.png *.ico

echo Done.
