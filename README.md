# Linux Game Jam 2019 entry

This is an entry for the Linux Game Jam 2019. Further details will be added as the project reaches
completion.

## Dependencies

In order to build the game you need SDL2, SDL2-mixer and libncurses. In addition you need to
install the Meson build tool and the build-essentials (GCC).

You can install them with the following command on Debian/Ubuntu:
```sh
sudo apt install meson build-essential libsdl2-dev libsdl2-mixer-dev libncurses-dev
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

You can then launch the game with the following command:

```
./build/platformer_game_pdcurses
```

## Credits

* __Samsai__ - Things and stuff
* __Tuubi__  - Odd jobs
* __Ysblokje__ - pdcurses integration etc.


#### Music and sound effects:

> __Midnight Tale__ by __Kevin MacLeod__ (incompetech.com)  
> Licensed under Creative Commons: By Attribution 3.0 License  
> http://creativecommons.org/licenses/by/3.0/

CC Attribution-licensed sound effects by freesound.org users:
> jorickhoofd, javapimp

The rest of the included audio was originally released under the CC 0-license.
