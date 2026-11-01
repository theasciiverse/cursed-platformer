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
- tmux has no key releases, so it tests the tap fallback; a pty that
  answers `CSI ? u` like kitty tests holding.
- Preview: `python3 asciiverse/record-preview.py` (asciinema 3 and pyte;
  `--dry --trace` to watch the bot think): the title screen, then Continue
  into Muncher Meadows, played to its exit by a bot in a pty that answers
  like kitty (real holds and releases). It waits for a muncher walking
  away to turn, then jumps it head-on (a jump is ~8 cells and 1 s; a
  muncher walks 4 cells/s; `>` walks left). Recorded with
  TERM=screen-256color, as pyte cannot read xterm's repeat-character
  escape. 24 s.
- Testing: `SDL_AUDIODRIVER=dummy` keeps the music off the speakers.
- Levels are plain text (`ascii-platformer/docs/level-editing.md`).
  Ian T. Jacobsen's four (`ian` to `ian3`, remote `ian`, his fork) are
  behind EXTENDED on the title screen (jump at the start). Our jungle
  episode (`jungle` ... `newdawn`, levels 11 to 19, Maya theme) follows the
  first ending; its end leads to the title screen.
- Check a level: `python3 asciiverse/levelcheck.py LEVEL` (every way of
  playing it, with the game's physics; needs cc). A jump rises just over 2
  cells, so a 2-high wall is easy and a 3-high one only works at the very
  top of a jump: keep new levels to 2. A jump from a moving platform is
  cancelled by the platform (upstream's behaviour), so platforms have to
  be walked off at their stops.
- Play a route in the real game: `python3 asciiverse/playtest.py ROUTE`
  (pyte; routes and their plans are in the script; it reads the game's F8
  debug line for exact positions). `episode` plays the jungle through.
