#! /bin/sh

meson -Duse_pdcurses=true --buildtype=release build
ninja -C build

mkdir -p platformer_package
cp build/platformer_game* platformer_package/

cp -r data platformer_package

mkdir -p platformer_package/libs
cd platformer_package

// This should include a minimal set of deps when building on Ubuntu/Debian
cp $(ldd platformer_game_ncurses | grep -E 'libtinfo|libncurses|libSDL2|libsndio|libfluidsynth|libreadline' |awk -F ' ' '{print $3}') libs/

cp ../run_ncurses.sh ./
cp ../run_pdcurses.sh ./

chmod +x run*.sh
