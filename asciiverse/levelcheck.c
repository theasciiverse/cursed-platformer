/* The search behind levelcheck.py: every way of playing a level, frame by
 * frame, with processPhysics()'s rules (src/entity.c) in exact units.
 * x is in fifteenths of a cell (a frame's walk at 8 cells/s is 2/15), y in
 * 1/3600 of a cell and y_vel in 1/60 cell/s (one frame's fall is y_vel/3600).
 *
 * stdin:  W H, then H rows of the map (one line each, '.' for a space);
 *         start x y; then entities, one per line, until "end":
 *           g x y      a goal          b x y   a button
 *           t x y tx ty a teleport     z x y w a trampoline
 * stdout: "goal i frames" for each goal reached, "button i" for each button
 *         stepped on, "states n".
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static int W, H;
static char map[64][256];
static int ng, nb, nt, nz;
static int gx[64], gy[64], gt[64];
static int bx[64], by[64], bhit[64];
static int tx[64], ty[64], ttx[64], tty[64];
static int zx[64], zy[64], zw[64];

static int passable(int x, int y) {
	if (x < 0 || y < 0 || x >= W || y >= H) return 0;
	char c = map[y][x];
	return !(c == '#' || c == '^' || c == ']' || c == '[');
}
static char tile(int x, int y) {
	if (x < 0 || y < 0 || x >= W || y >= H) return '#';
	return map[y][x];
}

/* a state: x 12 bits, y 20 bits, y_vel + 4096 13 bits, ground 1 bit */
#define PACK(x, y, v, g) (((uint64_t)(x) << 34) | ((uint64_t)(y) << 14) | ((uint64_t)((v) + 4096) << 1) | (uint64_t)(g))
static int hbits = 24;
static uint64_t *table, used;
static int put(uint64_t k) { /* k != 0 */
	uint64_t mask = (1ull << hbits) - 1, h = (k * 0x9E3779B97F4A7C15ull) >> (64 - hbits);
	for (;;) {
		if (table[h] == 0) { table[h] = k; return 1; }
		if (table[h] == k) return 0;
		h = (h + 1) & mask;
	}
}
static int insert(uint64_t k) {
	if (used * 2 > (1ull << hbits)) { /* grow at half full */
		if (hbits >= 28) { printf("toobig\n"); fflush(stdout); exit(3); }
		uint64_t *old = table, n = 1ull << hbits;
		hbits++;
		table = calloc(1ull << hbits, sizeof *table);
		if (!table) { fprintf(stderr, "out of memory\n"); exit(2); }
		for (uint64_t i = 0; i < n; i++) if (old[i]) put(old[i]);
		free(old);
	}
	if (put(k + 1)) { used++; return 1; }
	return 0;
}

typedef struct { uint64_t s; int t; long parent; } item;

int main(void) {
	if (scanf("%d %d", &W, &H) != 2) return 1;
	for (int y = 0; y < H; y++) {
		if (scanf("%255s", map[y]) != 1) return 1;
		for (int x = 0; x < W; x++) if (map[y][x] == '.') map[y][x] = ' ';
	}
	int sx, sy;
	if (scanf("%d %d", &sx, &sy) != 2) return 1;
	char kind[16];
	while (scanf("%15s", kind) == 1 && strcmp(kind, "end")) {
		if (!strcmp(kind, "g")) { if (scanf("%d %d", &gx[ng], &gy[ng]) != 2) return 1; gt[ng++] = -1; }
		else if (!strcmp(kind, "b")) { if (scanf("%d %d", &bx[nb], &by[nb]) != 2) return 1; bhit[nb++] = 0; }
		else if (!strcmp(kind, "t")) { if (scanf("%d %d %d %d", &tx[nt], &ty[nt], &ttx[nt], &tty[nt]) != 4) return 1; nt++; }
		else if (!strcmp(kind, "z")) { if (scanf("%d %d %d", &zx[nz], &zy[nz], &zw[nz]) != 3) return 1; nz++; }
	}
	table = calloc(1ull << hbits, sizeof *table);
	size_t cap = 1 << 20, head = 0, tail = 0;
	item *q = malloc(cap * sizeof *q);
	long states = 0;
	uint64_t s0 = PACK(sx * 15, sy * 3600, 0, 1);
	insert(s0);
	q[tail++] = (item){ s0, 0, -1 };
	int trace = getenv("TRACE_X") != NULL;
	int trx = trace ? atoi(getenv("TRACE_X")) : 0, try_ = trace ? atoi(getenv("TRACE_Y")) : 0;
	const int limit = 60 * 300;
	while (head < tail) {
		item it = q[head++];
		if (!trace && head > cap / 2) { /* reclaim the consumed front */
			memmove(q, q + head, (tail - head) * sizeof *q);
			tail -= head; head = 0;
		}
		states++;
		int x = (int)(it.s >> 34), y = (int)((it.s >> 14) & 0xFFFFF);
		int yv = (int)((it.s >> 1) & 0x1FFF) - 4096, ground = (int)(it.s & 1);
		int px = x / 15, py = y / 3600;
		if (trace && px == trx && py == try_) {
			for (long i = head - 1; i >= 0; i = q[i].parent) {
				uint64_t u = q[i].s;
				printf("trace %d %.3f %.3f %d\n", q[i].t, ((int)(u >> 34)) / 15.0,
					((int)((u >> 14) & 0xFFFFF)) / 3600.0, (int)((u >> 1) & 0x1FFF) - 4096);
			}
			return 0;
		}
		for (int i = 0; i < ng; i++) if (gt[i] < 0 && gx[i] == px && gy[i] == py) gt[i] = it.t;
		for (int i = 0; i < nb; i++) if (bx[i] == px && by[i] == py) bhit[i] = 1;
		for (int i = 0; i < nt; i++) if (tx[i] == px && ty[i] == py) {
			x = ttx[i] * 15; y = tty[i] * 3600; px = ttx[i]; py = tty[i]; break;
		}
		for (int i = 0; i < nz; i++)
			if (zx[i] <= px && px < zx[i] + zw[i] && y > zy[i] * 3600 - 1800 && y < zy[i] * 3600 && yv > 240)
				yv = -(yv * 9) / 10;
		if (it.t >= limit) continue;
		for (int vx = -2; vx <= 2; vx += 2) {
			for (int jump = 0; jump <= ground; jump++) {
				int nyv = jump ? -480 : yv;
				int ox = x / 15, oy = y / 3600, nx = x, ny = y;
				if (vx < 0 && !passable(ox - 1, oy)) nx = ox * 15;
				if (vx > 0 && !passable(ox + 1, oy)) nx = ox * 15;
				ny += nyv;
				nx += vx;
				if (nx < 0 || ny < 0) continue;
				int ix = nx / 15;
				if (!passable(ix, oy)) { nx = ox * 15; ix = ox; }
				int iy = ny / 3600;
				if (!passable(ix, iy)) { ny = oy * 3600; iy = oy; }
				if (passable(ix, iy + 1)) nyv += 4;
				char below = tile(ix, iy + 1);
				int ng2;
				if (below == '^') continue; /* dead */
				if ((!passable(ix, iy + 1) || below == '"' || below == 'T') && nyv >= 0) {
					ng2 = 1; nyv = 0; ny = iy * 3600;
				} else {
					nyv += 12; ng2 = 0;
				}
				if (iy >= H - 1 || nyv > 4000) continue;
				uint64_t s = PACK(nx, ny, nyv, ng2);
				if (insert(s)) {
					if (tail == cap) { cap *= 2; q = realloc(q, cap * sizeof *q); }
					q[tail++] = (item){ s, it.t + 1, (long)head - 1 };
				}
			}
		}
	}
	for (int i = 0; i < ng; i++) if (gt[i] >= 0) printf("goal %d %d\n", i, gt[i]);
	for (int i = 0; i < nb; i++) if (bhit[i]) printf("button %d\n", i);
	printf("states %ld\n", states);
	return 0;
}
