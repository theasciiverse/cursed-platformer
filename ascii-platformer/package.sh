#! /bin/bash

meson -Duse_pdcurses=true --buildtype=release build
ninja -C build

mkdir -p platformer_package
cp build/platformer_game* platformer_package/

cp -r data platformer_package

mkdir -p platformer_package/libs
pushd platformer_package

# This should include a minimal set of deps when building on Ubuntu/Debian
cp $(ldd platformer_game_ncurses | grep -E 'libtinfo|libncurses|libSDL2|libsndio|libfluidsynth|libreadline' |awk -F ' ' '{print $3}') libs/
cp -u $(ldd platformer_game_pdcurses | grep -E 'libSDL2' |awk -F ' ' '{print $3}') libs/

cp ../run_ncurses.sh ./
cp ../run_pdcurses.sh ./

chmod +x run*.sh
popd

DISTRO=`lsb_release -is`
DISTRO_VERSION=`lsb_release -sc`
echo \"${DISTRO_VERSION}\"
if [ "${DISTRO_VERSION}" = "n/a" ] ; then
    DISTRO_VERSION=`lsb_release -sr`
fi

tar cvzf "platformer_package-${DISTRO}-${DISTRO_VERSION}.tar.gz" platformer_package
