#!/bin/sh

set -xe

COMMON_CFLAGS="-Wall -Wextra -ggdb -I. -I./build/ -I./thirdparty/"

build_vc_demo() {
    NAME=$1

    clang $COMMON_CFLAGS -O2 -fno-builtin --target=wasm32 --no-standard-libraries -Wl,--no-entry -Wl,--export=render -Wl,--export=__heap_base -Wl,--allow-undefined -o ./build/$NAME.wasm -DPLATFORM=WASM_PLATFORM ./demos/$NAME.c
    mkdir -p ./wasm/
    cp ./build/$NAME.wasm ./wasm/

    clang $COMMON_CFLAGS -O2 -o ./build/$NAME.term -DPLATFORM=TERM_PLATFORM ./demos/$NAME.c -lm
}

mkdir -p ./build/

# Build assets
clang $COMMON_CFLAGS -o ./build/png2c png2c.c -lm 
mkdir -p ./build/assets/
./build/png2c -n nikita -o ./build/assets/nikita.c ./assets/nikita.png
./build/png2c -n Sadge -o ./build/assets/Sadge.c ./assets/Sadge.png

# Build tests
clang $COMMON_CFLAGS -fsanitize=memory -o ./build/test test.c -lm 

# Build VC demos
build_vc_demo triangle &
build_vc_demo 3d &
build_vc_demo squish &
build_vc_demo triangle3d &
wait
