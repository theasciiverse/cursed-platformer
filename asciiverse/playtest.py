#!/usr/bin/env python3
"""Plays levels in the real game, to check they can be finished as a player
would: the staged game (stage/) in a pseudo-terminal that answers like a
kitty-protocol terminal, so keys are held and let go, with the screen read
back through pyte. A plan per level says where to walk; the bot jumps when
a wall stops it and closes message boxes. Munchers are met simply and bats are not
dodged cleverly: see the plan steps below.

    python3 asciiverse/playtest.py [--trace] ROUTE

ROUTE names a route below: the levels it plays in order, starting from the
first (the game is started with it), each with its plan. A plan is a list
of steps:
    ("R", x)       walk right until x >= the given cell, jumping when stopped
    ("L", x)       the same, to the left
    ("J", x)       walk on (the last way) and jump when x is reached
    ("U",)         jump where the player stands, once on the ground
    ("S",)         stop walking (also in the air)
    ("G",)         wait until back on the ground after a jump or a fall
    ("Y", y)       walk on until the player has dropped to row y or below
    ("A", y)       wait until the player has risen to row y or above
    ("P", x1, x2, y1, y2)  wait until a moving platform is in that box
    ("W", s)       wait s seconds
On the ground the bot jumps a muncher coming at it and waits for one
walking away; after a death it starts the level again (up to five tries).
After the last step the bot keeps walking the last way until the level
changes. Exits non-zero if the route is not finished within its time."""
import fcntl, os, pty, re, select, struct, sys, tempfile, termios, time
import pyte

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GAME = os.path.join(ROOT, "stage", "cursed-platformer")
W, H, GW, GH = 100, 30, 80, 25
OX, OY = (W - GW) // 2, (H - GH) // 2 + 1
tracing = "--trace" in sys.argv

# up the cenote's ledges, from one side to the other
CLIMB_R = [("L", 50), ("S",), ("R", 51), ("J", 51), ("G",), ("S",)]
CLIMB_L = [("R", 58), ("S",), ("L", 57), ("J", 57), ("G",), ("S",)]
ROUTES = {
    # the title screen's EXTENDED, then Ian T. Jacobsen's four levels into
    # level 1
    "extended": [
        ("menu", [("U",)]),
        ("ian", [("R", 79)]),
        ("ian1", [("R", 79)]),
        ("ian2", [("R", 79)]),
        ("ian3", [("R", 78), ("W", 0.5), ("L", 66), ("R", 79)]),
        ("level0", []),
    ],
    # the jungle episode, after the pyramid's ending
    "ending": [("thatsallfolks", [("R", 79)]), ("jungle", [])],
    "jungle": [("jungle", [("J", 12), ("R", 79)]), ("canopy", [])],
    "canopy": [("canopy", [("J", 6), ("J", 15), ("J", 23), ("S", 28), ("G",), ("R", 31),
                           ("J", 31), ("J", 38), ("J", 47), ("S", 52), ("G",), ("R", 54),
                           ("J", 54), ("S", 59), ("G",), ("R", 62), ("J", 62), ("J", 69),
                           ("R", 79)]), ("cenote", [])],
    "cenote": [("cenote", [("R", 51), ("S",), ("U",), ("G",)] + CLIMB_R + CLIMB_L + CLIMB_R
                + CLIMB_L + CLIMB_R + [("R", 79)]), ("ballcourt", [])],
    "ballcourt": [("ballcourt", [("R", 24), ("S",), ("U",), ("G",), ("R", 63), ("S",),
                                 ("U",), ("G",), ("R", 79)]), ("sunsteps", [])],
    "sunsteps": [("sunsteps", [("R", 79)]), ("temple", [])],
    "temple": [("temple", [("R", 76), ("Y", 10), ("L", 31), ("J", 30), ("L", 3), ("Y", 15),
                           ("R", 76), ("Y", 20), ("L", 0)]), ("glyphs", [])],
    "glyphs": [("glyphs", [("J", 11), ("R", 79)]), ("serpent", [])],
    "serpent": [("serpent", [("R", 52), ("S",), ("P", 53, 55, 12, 13), ("R", 54), ("S",),
                             ("A", 5), ("R", 79)]), ("newdawn", [])],
    "newdawn": [("newdawn", [("R", 79)]), ("menu", [])],
}
# the whole episode, from the first ending to the title screen
ROUTES["episode"] = ROUTES["ending"][:1] + [ROUTES[n][0] for n in (
    "jungle", "canopy", "cenote", "ballcourt", "sunsteps", "temple", "glyphs",
    "serpent", "newdawn")] + [("menu", [])]

T0 = time.time()
def log(*a):
    if tracing:
        print("%6.2f" % (time.time() - T0), *a, flush=True)

def run(route, limit=600.0):
    levels = ROUTES[route]
    save = tempfile.mkdtemp()
    env = dict(os.environ, TERM="screen-256color", SDL_AUDIODRIVER="dummy",
               XDG_DATA_HOME=save)
    argv = [GAME] + ([levels[0][0]] if levels[0][0] != "menu" else [])
    pid, fd = pty.fork()
    if pid == 0:
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
                if b"\x1b[?u" in data:             # kitty, yes
                    os.write(fd, b"\x1b[?0u\x1b[?62c")
                stream.feed(data)
            if time.time() >= end:
                return True

    KEYS = {"f8": "19;1:{}~", "right": "1;1:{}C", "left": "1;1:{}D", "up": "1;1:{}A",
            "space": "32;1:{}u", "r": "114;1:{}u", "q": "113;1:{}u"}
    held = set()
    def press(k):
        os.write(fd, ("\x1b[" + KEYS[k].format(1)).encode()); held.add(k)
    def release(k):
        if k in held:
            os.write(fd, ("\x1b[" + KEYS[k].format(3)).encode()); held.discard(k)
    def tap(k, t=0.05):
        press(k); pump(t); release(k)

    def player():
        rs = screen.display
        for y in range(OY, OY + GH - 1):
            x = rs[y].find("@", OX, OX + GW)
            if x >= 0 and "says" not in rs[y]:
                return x - OX, y - OY
        return None

    titles = {}
    for name, _ in levels:
        meta = open(os.path.join(ROOT, "ascii-platformer", "data", name, "metadata.txt")).read()
        t = [l[6:].strip() for l in meta.split("\n") if l.startswith("title ")]
        titles[name] = ("LEVEL " + t[0])[:40] if t else None

    pump(2.0)
    tap("f8")       # the game's debug line: the player's exact place and state
    pump(0.2)
    li, step, way, last, stuck_since, ok = 0, 0, None, None, None, False
    tries, mx_seen, step_t, moved = 1, [], time.time(), time.time()
    start = time.time()

    def go(want):
        nonlocal way
        if way != want:
            if way:
                release(way)
            if want:
                press(want)
            way = want

    while time.time() - start < limit:
        if not pump(0.004):
            log("the game closed")
            break
        rs = screen.display
        title = rs[OY - 1][OX:OX + 36].rstrip()
        # the next level of the route has begun
        if li + 1 < len(levels) and titles[levels[li + 1][0]] and title.startswith(titles[levels[li + 1][0]][:20]):
            li += 1
            step, stuck_since, mx_seen, tries = 0, None, [], 1
            log("level", levels[li][0], "after", round(time.time() - start, 1), "s")
            if li == len(levels) - 1:
                ok = True
                break
            go(None)
        if any("(A) / Space" in r for r in rs):     # a message box
            w = way
            go(None)
            pump(0.6)
            tap("space")
            pump(0.2)
            go(w)
            continue
        dbg = re.search(r"X:([0-9.]+) +Y:([0-9.]+) +ST:([A-Z]+)", rs[OY - 1])
        p = player()
        if dbg and dbg.group(3) == "DEAD":
            p = None
        if p is None:
            if any("X" in r[OX:OX + GW] for r in rs[OY:OY + GH - 1]):
                log("died near", last, "in", levels[li][0], "at step", step)
                if tries >= 5:
                    break
                tries += 1
                go(None)
                pump(0.8)
                tap("r")
                pump(0.5)
                step, stuck_since, mx_seen, last = 0, None, [], None
            continue
        x, y = p
        if dbg:
            x, y = int(float(dbg.group(1))), int(float(dbg.group(2)))
            ground = dbg.group(3) == "WALKING"
        else:
            below = rs[OY + y + 1][OX + x] if OY + y + 1 < len(rs) else "#"
            ground = below in '#"T[]'
        plan = levels[li][1]
        if (x, y) != last:
            log(levels[li][0], "at", (x, y), "step", step)
            if last is None or y != last[1]:
                moved = time.time()             # the last change of row
            last, stuck_since = (x, y), None
        elif way and ground:
            if stuck_since is None:
                stuck_since = time.time()
            elif time.time() - stuck_since > 0.15:
                # not up onto a ledge while a muncher is at its edge
                near = False
                for yy in (y - 2, y - 1):
                    for k in range(1, 4):
                        xx = OX + x + (k if way == "right" else -k)
                        if 0 <= OY + yy and OX <= xx < OX + GW and rs[OY + yy][xx] in "<>-":
                            near = True
                if not near:
                    log("stopped at", (x, y), ": jump")
                    tap("up")
                    stuck_since = None

        # a muncher on the player's row, ahead: jump it when it comes,
        # wait while it walks away
        if ground and way:
            d = None
            row = rs[OY + y]
            for k in range(1, 8):
                xx = OX + x + (k if way == "right" else -k)
                if OX <= xx < OX + GW and row[xx] in "<>-":
                    d = k
                    break
            if d is None:
                mx_seen = []
            else:
                mx = x + (d if way == "right" else -d)
                if mx_seen and abs(mx_seen[-1] - mx) > 2:
                    mx_seen = []
                if not mx_seen or mx_seen[-1] != mx:
                    mx_seen.append(mx)
                coming = None
                if len(mx_seen) >= 2:
                    coming = abs(mx_seen[-1] - x) < abs(mx_seen[-2] - x)
                if coming and 2 <= d <= 4:
                    tap("up")
                    log("jump a muncher at", (x, y))
                    mx_seen = []
                    continue
                if coming is False and d <= 3:
                    w = way
                    go(None)
                    pump(0.25)
                    go(w)
                    continue

        if step < len(plan):
            s = plan[step]
            nxt = False
            if s[0] in "RL":
                go("right" if s[0] == "R" else "left")
                nxt = (s[0] == "R" and x >= s[1]) or (s[0] == "L" and x <= s[1])
            elif s[0] == "J":
                if way is None:
                    go("right")
                if (way == "right" and x >= s[1]) or (way == "left" and x <= s[1]):
                    tap("up")
                    nxt = True
            elif s[0] == "U":
                if ground:                      # a jump counts only from the ground
                    tap("up")
                    nxt = True
            elif s[0] == "S":
                go(None)
                nxt = True
            elif s[0] == "G":
                # landed: on something, and not moving up or down
                nxt = time.time() - step_t > 0.15 and ground
            elif s[0] == "Y":
                nxt = y >= s[1]
            elif s[0] == "A":
                nxt = y <= s[1]
            elif s[0] == "P":
                for yy in range(s[3], s[4] + 1):
                    if "T" in rs[OY + yy][OX + s[1]:OX + s[2] + 1]:
                        nxt = True
            elif s[0] == "W":
                go(None)
                pump(s[1])
                nxt = True
            if nxt:
                step += 1
                step_t = time.time()
                log("step", step, "done at", (x, y))
    for k in list(held):
        release(k)
    tap("q")
    pump(0.5)
    try:
        os.kill(pid, 15)
    except OSError:
        pass
    print(route + ":", "finished" if ok else "NOT finished",
          "in %.1f s" % (time.time() - start), "(last level %s)" % levels[li][0])
    return ok

if __name__ == "__main__":
    names = [a for a in sys.argv[1:] if not a.startswith("--")]
    sys.exit(0 if all([run(n) for n in names]) else 1)
