#!/usr/bin/env python3
"""Records the catalog preview, asciiverse/cursed-platformer.cast (100x30,
asciinema 3, tmux): the main menu, a new game, and a hands-off run through
the first level by a small bot that reads the screen, walks right, jumps
when a wall is ahead and closes the messages, up to the climb to its
switch. Prints the levels it saw.

    python3 asciiverse/record-preview.py [--dry]   (--dry: no recording)

Needs tmux and asciinema 3; the game is the staged one (stage/), run with
SDL's silent audio driver and a scratch save folder."""
import os, re, subprocess, sys, tempfile, time

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GAME = os.path.join(ROOT, "stage", "cursed-platformer")
CAST = os.path.join(ROOT, "asciiverse", "cursed-platformer.cast")
W, H, GW, GH = 100, 30, 80, 25
OX, OY = (W - GW) // 2, (H - GH) // 2
SESSION = "cp-preview"
dry = "--dry" in sys.argv
LENGTH = float(os.environ.get("PREVIEW_SECONDS", "45"))

def tmux(*a):
    return subprocess.run(["tmux", *a], capture_output=True, text=True).stdout

def screen():
    return tmux("capture-pane", "-p", "-t", SESSION).split("\n")

def key(k):
    tmux("send-keys", "-t", SESSION, k)

def find_player(rows):
    for y in range(OY + 1, min(OY + GH, len(rows))):
        x = rows[y].find("@", OX, OX + GW)
        if x >= 0 and "says" not in rows[y]:
            return x, y
    return None

def solid(c):
    return c in "#^[]"

save = tempfile.mkdtemp()
env = f"SDL_AUDIODRIVER=dummy XDG_DATA_HOME={save}"
cmd = f"cd /tmp && env {env} {GAME}"
if not dry:
    if os.path.exists(CAST):
        os.remove(CAST)
    cmd = (f"asciinema rec -q --overwrite --window-size {W}x{H} -i 2 "
           f"-c '{cmd}' {CAST}")
tmux("kill-session", "-t", SESSION)
tmux("new-session", "-d", "-s", SESSION, "-x", str(W), "-y", str(H), cmd)
time.sleep(2.5)                                     # the menu
key("Right")                                        # walk into NEW GAME
start = time.time()
levels, last, still, trace = [], None, 0, []
while time.time() - start < LENGTH:
    rows = screen()
    title = rows[OY][OX:OX + 50].strip() if len(rows) > OY else ""
    if title.startswith("LEVEL") and (not levels or levels[-1] != title):
        levels.append(title)
    if any("(A) / Space" in r for r in rows):
        time.sleep(1.6)                             # let it be read
        key("Space")
        time.sleep(0.3)
        continue
    p = find_player(rows)
    if p is None:
        time.sleep(0.05)
        continue
    x, y = p
    trace.append(p)
    ahead = rows[y][x + 1] if x + 1 < len(rows[y]) else "#"
    if solid(ahead):
        key("Space")                                # jump the wall
    # a tap of Right starts the walk and the next one stops it (tmux has
    # no key releases), so tap only when he has stood still a while
    still = still + 1 if p == last else 0
    if still == 6:
        key("Right")
    if still > 30:                                  # stuck: jump and go
        key("Space")
        key("Right")
        still = 0
    # the climb up the " blocks to level 1's switch is a puzzle, past this
    # bot: the preview ends a moment after he reaches it
    if x - OX >= 61 and levels and levels[-1].startswith("LEVEL 1"):
        time.sleep(1.5)
        break
    last = p
    time.sleep(0.05)
key("q")
time.sleep(1)
tmux("kill-session", "-t", SESSION)
print("levels seen:", levels)
if "--trace" in sys.argv:
    print(trace[::10])
if not dry:
    # the header names the command asciinema ran, with this machine's
    # paths: only the game's name is kept
    import json
    with open(CAST) as f:
        head, rest = f.readline(), f.read()
    h = json.loads(head)
    h["command"] = "cursed-platformer"
    h.pop("env", None)
    with open(CAST, "w") as f:
        f.write(json.dumps(h, separators=(",", ":")) + "\n" + rest)
    print("cast:", CAST, os.path.getsize(CAST), "bytes")
