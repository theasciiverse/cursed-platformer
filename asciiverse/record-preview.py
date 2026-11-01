#!/usr/bin/env python3
"""Records the catalog preview, asciiverse/cursed-platformer.cast (100x30,
asciinema 3): the title screen, then Continue into Muncher Meadows (a save
made for the recording), played through to its exit by a small bot.

asciinema runs in a pseudo-terminal of this script's own, which answers the
game like a terminal with the kitty keyboard protocol, so the bot can hold
and let go of keys as a player does (src/av_input.c). It reads the player's
place off the screen (pyte), walks, waits for munchers to come at him and
jumps them, and jumps, stops and drops where the level's plan says.

    python3 asciiverse/record-preview.py [--dry] [--trace]
      --dry    run the game without recording
      --trace  print the bot's decisions

Needs asciinema 3 and pyte; plays the staged game (stage/) with SDL's
silent audio driver and a scratch save folder."""
import fcntl, json, os, pty, select, struct, sys, tempfile, termios, time
import pyte

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GAME = os.path.join(ROOT, "stage", "cursed-platformer")
CAST = os.path.join(ROOT, "asciiverse", "cursed-platformer.cast")
W, H, GW, GH = 100, 30, 80, 25
OX, OY = (W - GW) // 2, (H - GH) // 2 + 1   # top-left of the level map
dry, tracing = "--dry" in sys.argv, "--trace" in sys.argv
LIMIT = 90.0

T0 = time.time()
def log(*a):
    if tracing:
        print("%5.1f" % (time.time() - T0), *a)

# the save Continue loads
save = tempfile.mkdtemp()
os.makedirs(os.path.join(save, "cursed-platformer"))
with open(os.path.join(save, "cursed-platformer", "autosave.sav"), "w") as f:
    f.write("level1\n")
# screen-256color: no "repeat character" escape (pyte cannot read it)
env = dict(os.environ, TERM="screen-256color", SDL_AUDIODRIVER="dummy",
           XDG_DATA_HOME=save)
argv = ([GAME] if dry else
        ["asciinema", "rec", "-q", "--overwrite", "-i", "2", "-c", GAME, CAST])

pid, fd = pty.fork()
if pid == 0:
    os.chdir("/tmp")
    os.execvpe(argv[0], argv, env)
fcntl.ioctl(fd, termios.TIOCSWINSZ, struct.pack("HHHH", H, W, 0, 0))
screen = pyte.Screen(W, H)
stream = pyte.ByteStream(screen)

def pump(t=0.0):
    end = time.time() + t
    while True:
        r, _, _ = select.select([fd], [], [], max(0.0, min(0.005, end - time.time())))
        if r:
            try:
                data = os.read(fd, 65536)
            except OSError:
                return False
            if b"\x1b[?u" in data:                 # the game asks: kitty, yes
                os.write(fd, b"\x1b[?0u\x1b[?62c")
            stream.feed(data)
        if time.time() >= end:
            return True

KEYS = {"right": "1;1:{}C", "left": "1;1:{}D", "space": "32;1:{}u", "r": "114;1:{}u", "q": "113;1:{}u"}
held = set()
def press(k):
    os.write(fd, ("\x1b[" + KEYS[k].format(1)).encode()); held.add(k)
def release(k):
    if k in held:
        os.write(fd, ("\x1b[" + KEYS[k].format(3)).encode()); held.discard(k)
def tap(k, t=0.06):
    press(k); pump(t); release(k)

def rows():
    return screen.display

def player(rs):
    for y in range(OY, OY + GH - 1):
        x = rs[y].find("@", OX, OX + GW)
        if x >= 0 and "says" not in rs[y]:
            return x - OX, y - OY
    return None

def title(rs):
    return rs[OY - 1][OX:OX + 50].strip()

def muncher_ahead(rs, x, y):
    row = rs[y + OY]
    for d in range(1, 21):
        if OX + x + d < len(row) and row[OX + x + d] in "<>-":
            return d
    return None

# Muncher Meadows: where to jump, stop and drop (game cells)
JUMPS = {(19, 20), (43, 18), (58, 16)}

pump(2.6)                                       # the title screen
tap("left", 0.5)                                # walk into CONTINUE
start = time.time()
levels, trace, mx_seen, phase = [], [], [], "walk"
while time.time() - start < LIMIT:
    if not pump(0.004):
        log("the game's terminal closed at", round(time.time() - start, 2), "s")
        break
    rs = rows()
    t = title(rs)
    if t.startswith("LEVEL") and (not levels or levels[-1] != t):
        levels.append(t)
        log("level:", t)
    if any("(A) / Space" in r for r in rs):     # a message: let it be read
        pump(1.8)
        tap("space")
        pump(0.15)
        continue
    if not t.startswith("LEVEL 2"):
        if any(l.startswith("LEVEL 2") for l in levels) and t.startswith("LEVEL"):
            pump(3.0)                           # the next level has begun
            break
        continue
    p = player(rs)
    if p is None:                               # dead (an X): try again
        if any("X" in r[OX:OX + GW] for r in rs[OY:OY + GH - 1]):
            log("died; last places:", trace[-5:])
            release("right")
            pump(1.0)
            tap("r")
            phase, mx_seen = "walk", []
        continue
    x, y = p
    if not trace or trace[-1][1:] != (x, y):
        trace.append((round(time.time() - start, 2), x, y))
    below = rs[OY + y + 1][OX + x] if OY + y + 1 < len(rs) else "#"
    ground = below in '#"T'                    # standing on something solid
    if y >= 21 and phase == "walk":             # in the water: try again
        log("fell in at", (x, y))
        release("right")
        tap("r")
        mx_seen = []
        pump(0.5)
        continue

    if phase == "drop":                         # stepping off the last ledge
        if y == 21:
            phase = "exit"
            press("right")
        continue
    if phase == "exit":
        continue

    # the nearest muncher ahead, and its way from its own place over time
    d = muncher_ahead(rs, x, y)
    mx = x + d if d is not None else None
    if mx is None or (mx_seen and abs(mx - mx_seen[-1][1]) > 3):
        mx_seen = []                            # none, or another muncher
    if mx is not None and (not mx_seen or mx_seen[-1][1] != mx):
        mx_seen.append((time.time(), mx))
    coming = None
    if len(mx_seen) >= 2:
        coming = mx_seen[-1][1] < mx_seen[-2][1]
    if (x, y) in JUMPS and ground:
        tap("space")
        log("plan jump at", (x, y))
        continue
    if (x, y) == (73, 14):                      # the end of the last ledge
        release("right")
        pump(0.3)
        tap("right", 0.12)                      # one step off, then fall
        phase = "drop"
        log("drop")
        continue
    if d is not None and ground:
        if coming is False or (coming is None and d > 6):
            if "right" in held:                 # it walks away: wait for it
                release("right")
                log("wait at", (x, y), "muncher", d, "ahead")
        elif coming and 2 <= d <= 6:
            press("right")
            tap("space")
            log("jump at", (x, y), "muncher", d, "ahead, coming")
            mx_seen = []
            continue
        elif coming and d > 6 and "right" in held:
            release("right")                    # let it come closer
    elif "right" not in held:
        press("right")

release("right")
tap("q")
pump(1.5)
try:
    os.kill(pid, 15)
except OSError:
    pass
print("levels seen:", levels)
if not dry and os.path.exists(CAST):
    # the header names the command asciinema ran, with this machine's
    # paths: only the game's name is kept
    with open(CAST) as f:
        head, rest = f.readline(), f.read()
    h = json.loads(head)
    h["command"] = "cursed-platformer"
    h.pop("env", None)
    with open(CAST, "w") as f:
        f.write(json.dumps(h, separators=(",", ":")) + "\n" + rest)
    print("cast:", CAST, os.path.getsize(CAST), "bytes")
