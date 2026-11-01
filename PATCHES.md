# Asciiverse patches

Cursed Pl@tformer by Samsai (Sami Lahtinen), Tuubi and Ysblokje (Minze
Zwerver), an entry in the Linux Game Jam 2019, from
[Samsai/linux-game-jam-2019-game](https://gitlab.com/Samsai/linux-game-jam-2019-game)
at its `1.5` tag, packaged for the Asciiverse in its terminal version:
ncurses, with SDL for the sound and the gamepad. Branch `asciiverse`;
upstream is the `upstream` remote.

The changes are small and meant to be offered upstream. The music and the
sounds are all upstream's, and so are levels 0 to 10; the extra levels are
listed below.

## Fixes

- **Build with current compilers.** `helper.c` defined `mainwin` again
  beside `main.c`'s; GCC 10 and later refuse to link that ("multiple
  definition"). It is declared `extern` there now.
- **Sounds.** Six sounds were loaded into a five-entry array, so the
  trampoline's sound was written past its end; `closeAudio()` freed
  `sizeof(sounds)` entries (the array's size in bytes, 48) when the game
  quit; and the audio device was opened twice at start. (`src/sound.c`)
- **Message boxes.** Their wait, `(long) (1.0 / 60) * 1000`, is 0, so a
  message box spun on one core; it now sleeps a sixtieth of a second. It
  also read an SDL event when there was none to read. They were made with
  `subwin()`, which takes screen coordinates, and sat a row off the play
  area; `derwin()` places them within it. (`src/helper.c`)
- **The gamepad** was opened into a local variable in `initSDL()`, so the
  global one stayed empty. (`src/main.c`)

## Changes

- **Hold to walk** (`src/av_input.c`, new). A terminal never says when a
  key goes up, so an arrow tap starts walking and the next tap stops it.
  Terminals that speak the
  [kitty keyboard protocol](https://sw.kovidgoyal.net/kitty/keyboard-protocol/)
  (kitty, Ghostty, WezTerm, foot, iTerm2, Alacritty and others) report
  presses and releases when asked: there an arrow walks while held and
  stops when let go. Elsewhere the taps work as before. The protocol is
  switched off again on exit and on SIGINT, SIGTERM and SIGHUP.
- **Gamepad.** The left stick walks as the d-pad does, d-pad up jumps as
  the Up key does, Back quits, and a pad plugged in while the game runs is
  taken up. SDL's messages ("Gamepad Found") go to stderr only when it is
  not the terminal; they were drawn over the game.
- **Saves.** Progress (`autosave.sav`) is kept in
  `${XDG_DATA_HOME:-~/.local/share}/cursed-platformer` instead of the
  folder the game runs in, and a missing or unwritable save file no longer
  crashes the game.
- **Centred.** The 80x25 game is drawn in the middle of a bigger terminal,
  and centred again when the terminal is resized.

## More levels

- **EXTENDED**, on the title screen: jump from where you start. It plays
  four levels by Ian T. Jacobsen, from his fork
  ([Smilex1/linux-game-jam-2019-game](https://gitlab.com/Smilex1/linux-game-jam-2019-game)),
  numbered -4 to -1, into level 1. They come as he made them, with the
  bridge his last level uses (`bridge x y dir`: a wall that grows across a
  gap once a button opens the way, in `src/entity.c` and `src/level.c`)
  and colour pairs 8 and 9 for his colour maps; in his name, from his
  commit. Only the titles' numbers are ours.
- **The jungle**: nine levels of ours (`jungle`, `canopy`, `cenote`,
  `ballcourt`, `sunsteps`, `temple`, `glyphs`, `serpent`, `newdawn`),
  numbered 11 to 19. The pharaoh's map leads across the sea to a jungle
  and up a Maya stepped pyramid. The first ending's exit leads in, where it
  used to go back to level 1; the jungle's ending leads to the title screen.
- `asciiverse/levelcheck.py` (with `levelcheck.c`) searches every way of
  playing a level with the game's physics and says which exits can be
  reached; `asciiverse/playtest.py` plays routes of levels in the real
  game. Every level here passes both.

## Packaging (new files)

- `asciiverse/build.sh`: builds the game with SDL2 2.32.10 and SDL2_mixer
  2.8.1 from source, linked in statically (SDL2_mixer with only its own Ogg
  Vorbis decoder), and on Linux ncursesw statically too. SDL still loads
  the system's sound library and udev at run time, so Linux builds are
  glibc, made on Ubuntu 22.04. Upstream's Meson build is left as it was.
- `asciiverse/cursed-platformer`: the start script, which runs the game
  from the folder holding `data/`.
- `asciiverse/stage.sh`, `asciiverse/nfpm.yaml`: the av archive and the
  .deb, .rpm and Arch packages.
- `asciiverse/record-preview.py`: records the catalog preview (a bot
  plays Muncher Meadows).
- `.github/workflows/release.yml`: builds and releases it all on a tag.

## Licences

The game is GPL-3.0 (LICENSE). "Midnight Tale" is by Kevin MacLeod
(incompetech.com), CC BY 3.0; sound effects are by freesound.org users
jorickhoofd and javapimp (CC BY) and CC0 (README.md). The Amstrad PC1512
font is VileR's, CC BY-SA 4.0; it is only used by the SDL window version
and is not in the av archive.
