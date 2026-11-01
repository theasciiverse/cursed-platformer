# Cursed Pl@tformer (Asciiverse packaging)

Cursed Pl@tformer by Samsai, Tuubi and Ysblokje, a curses platformer from
the Linux Game Jam 2019 (GPL-3.0; Kevin MacLeod's music CC BY 3.0, sound
effects CC BY and CC0), packaged for the Asciiverse catalog in its terminal
version, with SDL for the sound and the gamepad. Not our code: keep changes
small, list every one in PATCHES.md, and prefer patches upstream could take.

- Branch `asciiverse` on upstream's `1.5` tag (= their master, last commit
  2019-05-12); `upstream` is gitlab.com/Samsai/linux-game-jam-2019-game.
  Releases are tagged `v1.5-av.<n>`.
- Source is in `ascii-platformer/`; our additions there are
  `src/av_input.c` (kitty keyboard protocol: hold to walk) and small marked
  edits in `main.c`, `helper.c`, `entity.c`, `sound.c`.
- Build: `sh asciiverse/build.sh` (SDL2 and SDL2_mixer from source into
  av-build/, linked statically); for the release's glibc,
  `docker run --rm -v "$PWD":/src -w /src ubuntu:22.04 sh asciiverse/build.sh`.
  Then `sh asciiverse/stage.sh` and run `stage/cursed-platformer`.
  Upstream's Meson build (`cd ascii-platformer && meson setup build`) still
  works for quick tests.
- The game reads `data/` in the folder it runs in (the start script runs it
  from `lib/`) and saves progress to ~/.local/share/cursed-platformer.
- Testing: run with `SDL_AUDIODRIVER=dummy`, or the music plays on the
  machine's speakers. tmux has no key releases, so it tests the tap
  fallback; a pty that answers `CSI ? u` like kitty tests holding.
- Preview: `python3 asciiverse/record-preview.py` (tmux, asciinema 3): a
  bot walks level 1 up to the climb to its switch.
- Levels are plain text (`ascii-platformer/docs/level-editing.md`). A fork
  by Ian T. Jacobsen (gitlab.com/Smilex1/linux-game-jam-2019-game) adds four
  levels of his own, `ian` to `ian3`; not taken in so far.
