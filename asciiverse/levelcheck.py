#!/usr/bin/env python3
"""Checks that a level can be finished: a breadth-first search over every
way of playing it, frame by frame, with the game's own physics
(processPhysics in src/entity.c, 60 frames a second, walk 8 cells/s, jump
-8, gravity 16). Each frame the player may walk left, walk right or stand,
and jump when on the ground, as a terminal with key releases allows. The
search itself is levelcheck.c, built here on first use (needs cc).

    python3 asciiverse/levelcheck.py LEVEL [LEVEL...]   (from the repo root)

Buttons, bridges, teleports, trampolines and goals work as in the game; a
button pressed stays pressed, so the search runs again after each new one.
Munchers, bats and falling spikes are not modelled (a playtest meets them),
nor are moving platforms (reported). Prints the goals reached, each with
the quickest time to it, and exits non-zero if a level has none."""
import os, subprocess, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DATA = os.path.join(ROOT, "ascii-platformer", "data")
SRC = os.path.join(ROOT, "asciiverse", "levelcheck.c")
BIN = os.path.join(ROOT, "av-build", "levelcheck")

def passable(c):
    return c not in "#^]["

def load(name):
    d = os.path.join(DATA, name)
    meta = {"goal": [], "button": [], "teleport": [], "trampoline": [],
            "bridge": [], "platform": []}
    for line in open(os.path.join(d, "metadata.txt")):
        w = line.split()
        if not w or w[0] == "//":
            continue
        if w[0] == "size":
            meta["size"] = int(w[1]), int(w[2])
        elif w[0] == "start":
            meta["start"] = int(w[1]), int(w[2])
        elif w[0] == "goal":
            meta["goal"].append((int(w[1]), int(w[2]), w[3]))
        elif w[0] in meta:
            meta[w[0]].append(tuple(int(v) for v in w[1:]))
    W, H = meta["size"]
    rows = open(os.path.join(d, "map.txt")).read().split("\n")[:H]
    grid = [list(r.ljust(W)[:W]) for r in rows]
    return meta, grid

def search(meta, grid):
    W, H = meta["size"]
    lines = [f"{W} {H}"]
    lines += ["".join("." if c in " \t" else c for c in r) for r in grid]
    lines.append("%d %d" % meta["start"])
    lines += ["g %d %d" % g[:2] for g in meta["goal"]]
    lines += ["b %d %d" % b[:2] for b in meta["button"]]
    lines += ["t %d %d %d %d" % t for t in meta["teleport"]]
    lines += ["z %d %d %d" % z for z in meta["trampoline"]]
    lines.append("end")
    out = subprocess.run([BIN], input="\n".join(lines) + "\n", capture_output=True,
                         text=True).stdout
    if "toobig" in out:
        raise SystemExit("too many states to search (over 128 million)")
    reached, pressed, states = {}, set(), 0
    for l in out.split("\n"):
        w = l.split()
        if not w:
            continue
        if w[0] == "goal":
            reached[meta["goal"][int(w[1])][2]] = int(w[2])
        elif w[0] == "button":
            pressed.add(int(w[1]))
        elif w[0] == "states":
            states = int(w[1])
    return reached, pressed, states

def check(name):
    meta, grid = load(name)
    if meta["platform"]:
        print(f"{name}: has moving platforms, which this does not model")
    done = set()
    while True:
        reached, pressed, states = search(meta, grid)
        new = pressed - done
        if not new:
            break
        for i in new:
            bx, by, tx, ty = meta["button"][i]
            grid[ty][tx] = " "
            for gx, gy, d in meta["bridge"]:
                # a bridge grows into the gap the button opened, and on
                # while the way is clear
                x = gx + d
                if (x, gy) == (tx, ty):
                    while 0 <= x < len(grid[gy]) and passable(grid[gy][x]):
                        grid[gy][x] = "#"
                        x += d
        done |= new
    print(f"{name}: " + (", ".join(f"-> {g} in {t / 60:.1f} s" for g, t in reached.items())
                         if reached else "NO GOAL REACHABLE")
          + f"; buttons {sorted(meta['button'][i][:2] for i in done) or 'none'}; {states} states")
    return bool(reached)

if __name__ == "__main__":
    if not os.path.exists(BIN) or os.path.getmtime(BIN) < os.path.getmtime(SRC):
        os.makedirs(os.path.dirname(BIN), exist_ok=True)
        subprocess.run(["cc", "-O2", "-o", BIN, SRC], check=True)
    ok = all([check(n) for n in sys.argv[1:]])
    sys.exit(0 if ok else 1)
