# Cursed Pl@tformer 

Cursed Pl@tformer is a simple platformer game written using Curses and SDL2. It can be compiled as a terminal
application using ncurses or as a graphical application with pdcurses. Cursed Pl@tformer was originally
written as an entry in the Linux Game Jam 2019 and is licensed under GPLv3, except for the third-party assets 
which are detailed further in this document.

More information and pre-compiled binaries can be found on the Itch page: [https://samsai.itch.io/cursed-platformer](https://samsai.itch.io/cursed-platformer)  

## Dependencies

In order to build the game you need SDL2, SDL2-mixer, SDL2-ttf and libncurses. In addition you need to
install the Meson build tool and the build-essentials (GCC).

You can install them with the following command on Debian/Ubuntu:
```sh
sudo apt install meson build-essential libsdl2-dev libsdl2-mixer-dev libsdl2-ttf-dev libncurses-dev
```

On other distros, use your regional equivalents.

## Fetching from gitlab

```sh
git clone --recursive https://gitlab.com/Samsai/linux-game-jam-2019-game.git
```

## Building

The game uses the Meson build system.

To build the game run the following commands:

```sh
cd ascii-platformer && meson --buildtype=release build && ninja -C build
```

You can then launch the game with the following command:

```sh
./build/platformer_game_ncurses
```

If you want the pdcurses version:

```sh
cd ascii-platformer && meson -Duse_pdcurses=true --buildtype=release build && ninja -C build
```

If you want the mingw64 compiled version

```sh
cd ascii-platformer && meson -Duse_ncurses=false --cross=cross-mingw.txt -Duse_pdcurses=true --buildtype=release build && ninja -C build
```

You can then launch the game with the following command:

```
./build/platformer_game_pdcurses
```

If you want to create a tar.gz package which bundles the data and binaries (along with reasonable dependencies)
you can run the package.sh script:

```
./package.sh
```

This will copy the necessary binaries from the build folder, copy dependencies like SDL2 and create
run scripts for both the ncurses and pdcurses versions of the game in a folder called "platformer_package"
and create a tar.gz archive of that package. You can use this to make portable, distributable packages
of the game.

## Credits

* __Samsai__ - Things and stuff
* __Tuubi__  - Odd jobs
* __Ysblokje__ - pdcurses integration etc.


#### Assets:

> __Midnight Tale__ by __Kevin MacLeod__ (incompetech.com)  
> Licensed under Creative Commons: By Attribution 3.0 License  
> http://creativecommons.org/licenses/by/3.0/

CC Attribution-licensed sound effects by freesound.org users:
> jorickhoofd, javapimp

The rest of the included audio was originally released under the CC 0-license.

Font (for pdcurses):
> `Px437_AmstradPC1512-2y.ttf` from [The Ultimate Oldschool PC Font Pack](https://int10h.org/oldschool-pc-fonts/)  
> __(c) 2016 VileR__ / https://int10h.org  
> Licensed under [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/)

