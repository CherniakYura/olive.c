#!/bin/sh

set -xe

COMMON_CFLAGS="-Wall -Wextra -pedantic -std=c99 -ggdb -I. -I./build/ -I./dev-deps/"

build_wasm_demo() {
    NAME=$1
    clang $COMMON_CFLAGS -O2 -fno-builtin --target=wasm32 --no-standard-libraries -Wl,--no-entry -Wl,--export=vc_render -Wl,--export=__heap_base -Wl,--allow-undefined -o ./build/demos/$NAME.wasm -DVC_PLATFORM=VC_WASM_PLATFORM ./demos/$NAME.c
    mkdir -p ./wasm/
    cp ./build/demos/$NAME.wasm ./wasm/
}

build_term_demo() {
    NAME=$1
    clang $COMMON_CFLAGS -O2 -o ./build/demos/$NAME.term -DVC_PLATFORM=VC_TERM_PLATFORM -D_XOPEN_SOURCE=600 ./demos/$NAME.c -lm
}

build_vc_demo() {
    NAME=$1
    build_wasm_demo $NAME
    build_term_demo $NAME
}

build_all_vc_demos() {
    mkdir -p ./build/demos
    build_vc_demo triangle 
    build_vc_demo 3d 
    build_vc_demo squish 
    build_vc_demo triangle3d 
    build_vc_demo triangleTex 
    build_vc_demo triangle3dTex 
    build_vc_demo cup3d 
    wait
}

build_tools() {
    mkdir -p ./build/tools/
    clang $COMMON_CFLAGS -o ./build/tools/png2c -Ithirdparty ./tools/png2c.c -lm 
    clang $COMMON_CFLAGS -o ./build/tools/obj2c -Ithirdparty ./tools/obj2c.c -lm 
    clang $COMMON_CFLAGS -O2 -o ./build/tools/viewobj ./tools/viewobj.c 
    wait
}

build_assets() {
    mkdir -p ./build/assets/
    ./build/tools/png2c -n nikita -o ./build/assets/nikita.c ./assets/nikita.png 
    ./build/tools/png2c -n Sadge -o ./build/assets/Sadge.c ./assets/Sadge.png 
    ./build/tools/obj2c ./assets/cupLowPoly.obj ./build/assets/cupLowPoly.c 
    wait
}

build_tests() {
    clang $COMMON_CFLAGS -fsanitize=memory -o ./build/test -Ithirdparty ./test.c -lm 
}

build_tools
build_assets
build_tests
build_all_vc_demos
