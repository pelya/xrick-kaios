# xrick emscripten build script
# runs in build.tmp

# configure pathsg
[ -z "$PATH_PYTHON" ] && PATH_PYTHON="which python | xargs dirname"
[ -z "$PATH_EMSDK" ] && PATH_EMSDK="`which emsdk | xargs dirname`"
[ -z "$PATH_XRICK" ] && PATH_XRICK="`realpath $0 | xargs dirname`/../.."

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

emcc $SRC/*.c -o xrick.js \
    -I "$INC" \
    -s USE_SDL=2 \
    -s FORCE_FILESYSTEM=1 \
    -D NOZLIB \
    -O2 \
    --preload-file "$DATA@/data" \
    || exit 1

echo

echo Copy files...
if [ "`pwd`" != "$PATH_XRICK/build/emsdk" ]; then
    cp "$PATH_XRICK/build/emsdk/player.js" player.js
    cp "$PATH_XRICK/build/emsdk/index.html" index.html
    cp "$PATH_XRICK/build/emsdk/jquery-3.4.1.min.js" jquery-3.4.1.min.js
    cp "$PATH_XRICK/build/emsdk/style.min.css" style.min.css
    cp "$PATH_XRICK/build/emsdk/icon.png" icon.png
fi

echo Compress...
rm -rf  gz
mkdir gz
gzip -c -9 xrick.data > gz/xrick.data
gzip -c -9 xrick.wasm > gz/xrick.wasm
gzip -c -9 xrick.js > gz/xrick.js
gzip -c -9 player.js > gz/player.js
gzip -c -9 index.html > gz/index.html

echo Remember to:
echo Upload the files
echo Set the WASM Content-Type HTTP header to application/wasm
echo Set the DATA Content-Type HTTP header to application/octet-stream
echo Set the compressed Content-Encoding to gzip

echo Done.