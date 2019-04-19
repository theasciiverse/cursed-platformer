# Linux Game Jam 2019 entry

This is an entry for the Linux Game Jam 2019. Further details will be added as the project reaches
completion.

## Fetching from gitlab

```sh
git clone --recursive https://gitlab.com/Samsai/linux-game-jam-2019-game.git
```

## Building

The game uses the Meson build system.

To build the game run the following commands:

```sh
cd ascii-platformer && meson -Duse_pdcurses=true --buildtype=release build && ninja -C build
```

You can then launch the game with the following command:

```sh
./build/platformer_game_ncurses
```

You can also launch the pdcurses version like this:

```
./build/platformer_game_pdcurses
```

## Credits

* __Samsai__ - Things and stuff
* __Tuubi__  - Odd jobs
* __Ysblokje__ - pdcurses integration etc.


#### Music

> __Midnight Tale__ by __Kevin MacLeod__ (incompetech.com)  
> Licensed under Creative Commons: By Attribution 3.0 License  
> http://creativecommons.org/licenses/by/3.0/
