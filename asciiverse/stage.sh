#!/bin/sh
# Lays out the av archive in stage/: the start script at the top, the game
# in lib/ with its levels and sounds in lib/data/ (the game reads data/ in
# the folder it runs in). The TrueType font is left out: it is only for
# upstream's SDL window version.
set -eu
rm -rf stage && mkdir -p stage/lib
cp asciiverse/cursed-platformer stage/cursed-platformer
cp cursed-platformer stage/lib/cursed-platformer
cp -R ascii-platformer/data stage/lib/data
rm -f stage/lib/data/*.ttf
cp LICENSE README.md PATCHES.md stage/
