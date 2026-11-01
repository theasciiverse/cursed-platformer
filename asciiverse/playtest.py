#!/usr/bin/env python3
"""Plays levels in the real game, to check they can be finished as a player
would: the staged game (stage/) in a pseudo-terminal that answers like a
kitty-protocol terminal, so keys are held and let go, with the screen read
back through pyte. A plan per level says where to walk; the bot jumps when
a wall stops it and closes message boxes. Munchers and bats are not
dodged, so a plan for a level with them has to time its own way past.

    python3 asciiverse/playtest.py [--trace] ROUTE

ROUTE names a route below: the levels it plays in order, starting from the
first (the game is started with it), each with its plan. A plan is a list
of steps:
    ("R", x)       walk right until x >= the given cell, jumping when stopped
    ("L", x)       the same, to the left
    ("J", x)       walk on (the last way) and jump when x is reached
    ("U",)         jump where the player stands
    ("S",)         stop walking
    ("W", s)       wait s seconds
After the last step the bot keeps walking the last way until the level
changes. Exits non-zero if the route is not finished within its time."""
import fcntl, os, pty, select, struct, sys, tempfile, termios, time
import pyte

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GAME = os.path.join(ROOT, "stage", "cursed-platformer")
W, H, GW, GH = 100, 30, 80, 25
OX, OY = (W - GW) // 2, (H - GH) // 2 + 1
tracing = "--trace" in sys.argv

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
}

T0 = time.time()
def log(*a):
    if tracing:
        print("%6.2f" % (time.time() - T0), *a, flush=True)

def run(route, limit=240.0):
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

    KEYS = {"right": "1;1:{}C", "left": "1;1:{}D", "up": "1;1:{}A",
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
    li, step, way, last, stuck_since, ok = 0, 0, None, None, None, False
    start = time.time()
    while time.time() - start < limit:
        if not pump(0.004):
            log("the game closed")
            break
        rs = screen.display
        title = rs[OY - 1][OX:OX + 40].rstrip()
        # the next level of the route has begun
        if li + 1 < len(levels) and titles[levels[li + 1][0]] and title.startswith(titles[levels[li + 1][0]][:20]):
            li += 1
            step, stuck_since = 0, None
            log("level", levels[li][0], "after", round(time.time() - start, 1), "s")
            if li == len(levels) - 1:
                ok = True
                break
            for k in list(held):
                release(k)
            way = None
        if any("(A) / Space" in r for r in rs):     # a message box
            for k in list(held):
                release(k)
            pump(0.6)
            tap("space")
            pump(0.2)
            if way:
                press(way)
            continue
        p = player()
        if p is None:
            if any("X" in r[OX:OX + GW] for r in rs[OY:OY + GH - 1]):
                log("died near", last, "in", levels[li][0])
                break
            continue
        x, y = p
        rows = rs
        below = rows[OY + y + 1][OX + x] if OY + y + 1 < len(rows) else "#"
        ground = below in '#"T[]'
        plan = levels[li][1]
        if (x, y) != last:
            log(levels[li][0], "at", (x, y), "step", step)
            last, stuck_since = (x, y), None
        elif way and ground:
            if stuck_since is None:
                stuck_since = time.time()
            elif time.time() - stuck_since > 0.15:
                log("stopped at", (x, y), ": jump")
                tap("up")
                stuck_since = None
        if step < len(plan):
            s = plan[step]
            if s[0] in "RL":
                want = "right" if s[0] == "R" else "left"
                if way != want:
                    if way:
                        release(way)
                    press(want)
                    way = want
                if (s[0] == "R" and x >= s[1]) or (s[0] == "L" and x <= s[1]):
                    step += 1
            elif s[0] == "J":
                if (way == "right" and x >= s[1]) or (way == "left" and x <= s[1]) or way is None:
                    tap("up")
                    step += 1
            elif s[0] == "U":
                tap("up")
                step += 1
            elif s[0] == "S":
                if way:
                    release(way)
                way = None
                step += 1
            elif s[0] == "W":
                if way:
                    release(way)
                way = None
                pump(s[1])
                step += 1
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
