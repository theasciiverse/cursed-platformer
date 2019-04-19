#! /bin/sh

meson build
ninja -C build

mkdir -p platformer_package
cp build/platformer_game* platformer_package/

cp -r data platformer_package

mkdir -p platformer_package/libs
cd platformer_package

cp $(ldd platformer_game_ncurses | awk -F ' ' '{print $3}') libs/
cp $(ldd platformer_game_pdcurses | awk -F ' ' '{print $3}') libs/

echo '#! /bin/sh
LD_LIBRARY_PATH=./libs ./platformer_game_ncurses' > run_ncurses.sh

echo '#! /bin/sh
LD_LIBRARY_PATH=./libs ./platformer_game_pdcurses' > run_pdcurses.sh

chmod +x run*.sh
