#!/bin/sh
# Builds the terminal version of Cursed Pl@tformer (ncurses, with SDL for
# the sound and the gamepad) as ./cursed-platformer.
#
# SDL2 and SDL2_mixer are built from source into av-build/ and linked in
# statically, SDL2_mixer with only its own Ogg Vorbis decoder (all the
# game's sound is Ogg), so the binary needs no SDL installed. SDL still
# loads the system's sound library (PipeWire, PulseAudio or ALSA) and udev
# when the game starts, which a static musl binary cannot do, so Linux
# builds are glibc: run inside ubuntu:22.04 to run on glibc 2.35 and newer,
#   docker run --rm -v "$PWD":/src -w /src ubuntu:22.04 sh asciiverse/build.sh
# Linux links ncursesw statically too (Arch has no libtinfo.so.6); macOS
# links the system's ncurses, run on the runner with CC_ARCH set to x86_64
# or arm64.
set -eu
SDL2=2.32.10
SDL2_MIXER=2.8.1

if [ -f /etc/os-release ] && grep -q '^ID=ubuntu' /etc/os-release && [ "$(id -u)" = 0 ]; then
  export DEBIAN_FRONTEND=noninteractive
  apt-get update -qq
  apt-get install -y -qq build-essential cmake git pkg-config libncurses-dev \
    libasound2-dev libpulse-dev libpipewire-0.3-dev libudev-dev >/dev/null
fi

root=$(pwd)
deps="$root/av-build"
prefix="$deps/prefix"
mkdir -p "$deps"
arch=${CC_ARCH:+-DCMAKE_OSX_ARCHITECTURES=$CC_ARCH}
jobs=$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)

if [ ! -f "$prefix/lib/libSDL2.a" ]; then
  [ -d "$deps/SDL" ] || git clone -q --depth 1 -b "release-$SDL2" https://github.com/libsdl-org/SDL "$deps/SDL"
  # no video or rendering: the game draws with curses and uses SDL only for
  # its sound, its events and the gamepad
  cmake -S "$deps/SDL" -B "$deps/SDL-build" -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$prefix" -DSDL_SHARED=OFF -DSDL_STATIC=ON \
    -DSDL_TEST=OFF -DSDL_VIDEO=OFF -DSDL_RENDER=OFF -DSDL_DLOPEN=ON \
    -DSDL_STATIC_PIC=ON $arch >/dev/null
  cmake --build "$deps/SDL-build" -j "$jobs" >/dev/null
  cmake --install "$deps/SDL-build" >/dev/null
fi

if [ ! -f "$prefix/lib/libSDL2_mixer.a" ]; then
  [ -d "$deps/SDL_mixer" ] || git clone -q --depth 1 -b "release-$SDL2_MIXER" https://github.com/libsdl-org/SDL_mixer "$deps/SDL_mixer"
  cmake -S "$deps/SDL_mixer" -B "$deps/SDL_mixer-build" -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$prefix" -DCMAKE_PREFIX_PATH="$prefix" \
    -DBUILD_SHARED_LIBS=OFF -DSDL2MIXER_SAMPLES=OFF -DSDL2MIXER_VORBIS=STB \
    -DSDL2MIXER_OPUS=OFF -DSDL2MIXER_FLAC=OFF -DSDL2MIXER_MOD=OFF \
    -DSDL2MIXER_MIDI=OFF -DSDL2MIXER_MP3=OFF -DSDL2MIXER_WAVPACK=OFF \
    -DSDL2MIXER_GME=OFF $arch >/dev/null
  cmake --build "$deps/SDL_mixer-build" -j "$jobs" >/dev/null
  cmake --install "$deps/SDL_mixer-build" >/dev/null
fi

export PKG_CONFIG_PATH="$prefix/lib/pkgconfig"
sdl_cflags=$(pkg-config --cflags SDL2_mixer sdl2)
sdl_libs=$(pkg-config --static --libs SDL2_mixer sdl2)
if [ "$(uname)" = Darwin ]; then
  curses="-lncurses"
else
  curses="-Wl,-Bstatic -lncursesw -ltinfo -Wl,-Bdynamic"
fi

cd ascii-platformer
rm -f "$root/cursed-platformer"
# shellcheck disable=SC2086
cc${CC_ARCH:+ -arch $CC_ARCH} -std=gnu11 -O2 -D_DEFAULT_SOURCE $sdl_cflags \
  src/main.c src/level.c src/map.c src/enemy.c src/entity.c src/helper.c \
  src/sound.c src/av_input.c \
  $curses $sdl_libs -lm -o "$root/cursed-platformer"
strip "$root/cursed-platformer"
