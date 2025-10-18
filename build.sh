#!/bin/sh
set -e

build() {
	BUILD_DIR=$1
	TOOLCHAIN=$2

	mkdir -p "$BUILD_DIR"
	cd "$BUILD_DIR"

	if [ -n "$TOOLCHAIN" ]; then
		cmake .. -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN"
	else
		cmake ..
	fi

	cmake --build .
	cd - >/dev/null
}

build build-linux
build build-windows ../Windows.cmake

sha256sum build-linux/bfi > build-linux/bfi.sha256
sha256sum build-windows/bfi.exe > build-windows/bfi.exe.sha256
