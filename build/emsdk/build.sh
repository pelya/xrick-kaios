# xrick emscripten build script
# runs in build.tmp

# configure pathsg
[ -z "$PATH_PYTHON" ] && PATH_PYTHON="which python | xargs dirname"
[ -z "$PATH_EMSDK" ] && PATH_EMSDK="`which emsdk | xargs dirname`"
[ -z "$PATH_XRICK" ] && PATH_XRICK="`pwd`/../.."

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
for f in "$SRC"/*.c; \
  do emcc "$f" -o $(basename -- "${f%.c}").bc \
    -I "$INC" \
    -s USE_SDL=2 \
    -D NOZLIB \
    -O2 \
    ; \
done
echo

echo Linking...
# -o .wasm .html .js
emcc *.bc -o xrick.js \
  -s USE_SDL=2 \
  -s FORCE_FILESYSTEM=1 \
  -O2 \
  --preload-file "$DATA@/data"
echo

echo Copy files...
cp "$PATH_XRICK/build/emsdk/player.js" player.js

echo Compress...
rm -rf  gz
mkdir gz
gzip -c -9 xrick.data > gz/xrick.data
gzip -c -9 xrick.wasm > gz/xrick.wasm
gzip -c -9 xrick.js > gz/xrick.js
gzip -c -9 player.js > gz/player.js

echo Remember to:
echo Upload the files
echo Set the WASM Content-Type HTTP header to application/wasm
echo Set the DATA Content-Type HTTP header to application/octet-stream
echo Set the compressed Content-Encoding to gzip

echo Done.