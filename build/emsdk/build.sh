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
    --preload-file "$DATA" \
    || exit 1

echo

echo Copy files...
if [ "`pwd`" != "$PATH_XRICK/build/emsdk" ]; then
    cp "$PATH_XRICK/build/emsdk/player.js" player.js
    cp "$PATH_XRICK/build/emsdk/index.html" index.html
    cp "$PATH_XRICK/build/emsdk/jquery-3.4.1.min.js" jquery-3.4.1.min.js
    cp "$PATH_XRICK/build/emsdk/style.min.css" style.min.css
    cp "$PATH_XRICK/build/emsdk/icon.png" icon.png
    cp "$PATH_XRICK/build/emsdk/splash.png" splash.png
    cp "$PATH_XRICK/build/emsdk/xrick-b.png" xrick-b.png
    cp "$PATH_XRICK/build/emsdk/pad.png" pad.png
    cp "$PATH_XRICK/build/emsdk/fire.png" fire.png
    cp "$PATH_XRICK/build/emsdk/manifest.webapp" manifest.webapp
fi

rm -f application.zip
zip application.zip manifest.webapp *.data *.js *.html *.css *.png # *.wasm # WebAssembly not supported on KaiOS

echo Done.
