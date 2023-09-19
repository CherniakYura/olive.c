#!/bin/sh

set -xe

COMMON_CFLAGS="-Wall -Wextra -ggdb -I. -I./build/ -I./thirdparty/"

build_wasm_demo() {
    NAME=$1
    clang $COMMON_CFLAGS -O2 -fno-builtin --target=wasm32 --no-standard-libraries -Wl,--no-entry -Wl,--export=render -Wl,--export=__heap_base -Wl,--allow-undefined -o ./build/demos/$NAME.wasm -DPLATFORM=WASM_PLATFORM ./demos/$NAME.c
    mkdir -p ./wasm/
    cp ./build/demos/$NAME.wasm ./wasm/
}

build_term_demo() {
    NAME=$1
    clang $COMMON_CFLAGS -O2 -o ./build/demos/$NAME.term -DPLATFORM=TERM_PLATFORM ./demos/$NAME.c -lm
}

build_vc_demo() {
    NAME=$1
    build_wasm_demo $NAME
    build_term_demo $NAME
}

build_all_vc_demos() {
    mkdir -p ./build/demos
    build_vc_demo triangle &
    build_vc_demo 3d &
    build_vc_demo squish &
    build_vc_demo triangle3d &
    build_vc_demo triangleTex &
    build_vc_demo triangle3dTex &
    wait
}

build_assets() {
    mkdir -p ./build/assets/
    clang $COMMON_CFLAGS -o ./build/png2c png2c.c -lm
    ./build/png2c -n nikita -o ./build/assets/nikita.c ./assets/nikita.png
    ./build/png2c -n Sadge -o ./build/assets/Sadge.c ./assets/Sadge.png
}

build_tests() {
    mkdir -p ./build/
    clang $COMMON_CFLAGS -fsanitize=memory -o ./build/test test.c -lm
}

build_assets
build_tests
build_all_vc_demos
