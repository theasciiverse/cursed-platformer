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
cd ascii-platformer && meson build && ninja -C build
```

You can then launch the game with the following command:

```sh
./build/linux_game_jam_2019_game
```
