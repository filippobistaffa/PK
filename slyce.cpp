#include "pc.h"

typedef int16_t sign;
static size_t bcm[(N + 1) * (N + 1)], pm[N * N];

#define P(_s, _i) (pm[(_s) * N + (_i)])
#define BC(_n, _m) (bcm[(_n) * (N + 1) + (_m)])
#define LTDR(X, Y) (GET(dr, *(X)) == GET(dr, *(Y)) ? (*(X)) < (*(Y)) : GET(dr, *(X)) > GET(dr, *(Y)))
#define LEDR(X, Y) (GET(dr, *(X)) == GET(dr, *(Y)) ? (*(X)) <= (*(Y)) : GET(dr, *(X)) > GET(dr, *(Y)))

void filltables() {

	for (agent i = 0; i <= N; i++) BC(i, 0) = BC(i, i) = 1ULL;
	for (agent i = 1; i <= N; i++) for (agent j = 1; j < i; j++) BC(i, j) = BC(i - 1, j - 1) + BC(i - 1, j);
	for (agent i = 1; i < N; i++) P(i, 1) = 1ULL;
	for (agent i = 2; i < N; i++) P(1, i) = i;
	for (agent i = 2; i < N; i++) for (agent j = 2; j < N; j++) P(i, j) = P(i - 1, j) + P(i, j - 1);
}

__attribute__((always_inline)) inline
sign twiddle(sign *x, sign *y, sign *z, sign *p) {

	register sign i, j = 1, k;
	while (p[j] <= 0) j++;

	if (p[j - 1] == 0) {

		for (i = j - 1; i != 1; i--) p[i] = -1;

		p[j] = 0;
		*x = *z = 0;
		p[1] = 1;
		*y = j - 1;
	}
	else {
		if (j > 1) p[j - 1] = 0;

		do j++;
		while (p[j] > 0);

		k = j - 1;
		i = j;

		while (p[i] == 0) p[i++] = -1;

		if (p[i] == -1) {

			p[i] = p[k];
			*z = p[k] - 1;
			*x = i - 1;
			*y = k - 1;
			p[k] = -1;
		}
		else {
			if (i == p[0]) return 1;
			else {
				p[j] = p[i];
				*z = p[i] - 1;
				p[i] = 0;
				*x = j - 1;
				*y = i - 1;
			}
		}
	}

	return 0;
}

__attribute__((always_inline)) inline
void inittwiddle(sign m, sign n, sign *p) {

	sign i;
	p[0] = n + 1;

	for (i = 1; i != n - m + 1; i++) p[i] = 0;

	while (i != n + 1) {
		p[i] = i + m - n;
		i++;
	}

	p[n + 1] = -2;
	if (m == 0) p[1] = 1;
}

void printc(const agent *c, penny v) {

	agent n = *c;
	printf("{ ");
	while (n--) printf("%u ", *(++c));
	printf("} = %u\n", v);
}

template <typename type, typename out> __attribute__((always_inline)) inline
void unionsorted(const type *x, unsigned m, const type *y, unsigned n, type *z, out *o, const chunk *dr) {

	*o = 0;

	while (m && n) {
		if (LTDR(x, y)) { *(z++) = *(x++); m--; }
		else if (LTDR(y, x)) { *(z++) = *(y++); n--; }
		else { *(z++) = *(y++); x++; m--; n--; }
		(*o)++;
	}

	(*o) += m + n;
	if (m) memcpy(z, x, sizeof(type) * m);
	else memcpy(z, y, sizeof(type) * n);
}

template <typename type, typename out> __attribute__((always_inline)) inline
void differencesorted(const type *x, unsigned m, const type *y, unsigned n, type *z, out *o, const chunk *dr) {

	*o = 0;

	while (m && n) {
		if (LTDR(x, y)) { *(z++) = *(x++); m--; (*o)++; }
		else if (LTDR(y, x)) { y++; n--; }
		else { y++; x++; m--; n--; }
	}

	if (!m) return;
	(*o) += m;
	memcpy(z, x, sizeof(type) * m);
}

template <typename type> __attribute__((always_inline)) inline
unsigned binarysearch(type x, const type *buf, unsigned n, const chunk *dr) {

	if (n) {
		#define MIDPOINT(_min, _max) (_min + ((_max - _min) / 2))
		unsigned imin = 0, imid, imax = n - 1;

		while (imin < imax) {
			imid = MIDPOINT(imin, imax);
			if (LTDR(buf + imid, &x)) imin = imid + 1;
			else imax = imid;
		}

		if (imax == imin && buf[imin] == x) return imin;
	}
	return n + 1;
}

__attribute__((always_inline)) inline
void neighbours(const agent *f, agent m, const agent *adj, agent *n, const chunk *dr) {

	if (m) {
		agent t[N + 1];
		memcpy(n, adj + *f * N, sizeof(agent) * (adj[*f * N] + 1));
		f++;

		while (--m) {
			unionsorted(n + 1, *n, adj + *f * N + 1, adj[*f * N], t + 1, t, dr);
			memcpy(n, t, sizeof(agent) * (*t + 1));
			f++;
		}
	}
	else *n = 0;
}

__attribute__((always_inline)) inline
void nbar(const agent *f, agent n, const agent *r, const agent *ruf, const agent *adj, agent *nb, const chunk *dr) {

	agent a[N + 1], b[N + 1];
	neighbours(f, n, adj, a, dr);
	agent i = 0;
	while (i < *a && LEDR(a + i + 1, ruf + 1)) i++;
	memmove(a + 1, a + i + 1, sizeof(agent) * (*a - i));
	*a -= i;
	neighbours(r + 1, *r, adj, nb, dr);
	unionsorted(nb + 1, *nb, ruf + 1, *ruf, b + 1, b, dr);
	differencesorted(a + 1, *a, b + 1, *b, nb + 1, nb, dr);
}

__attribute__((always_inline)) inline
unsigned vectorsum(const agent *r, agent n, const chunk *x) {

	unsigned ret = 0;
	do {
		ret += GET(x, *r);
		r++;
	} while (--n);
	return ret;
}

__attribute__((always_inline)) inline
void coalition(agent *c, agent d, const meter *sp) {

}

size_t slyce(agent *r, agent *f, agent m, const agent *adj, agent d, const chunk *dr, const meter *sp) {

	size_t ret = 0;

	if (*r && (d || *r == 1)) {
		//coalition(r, d, sp);
		printc(r, COALVALUE(r, GET(dr, *(r + 1)), sp));
		ret++;
	}

	if (*f && m) {

		agent k, *nr = r + K + 1, *nf = f + N + 1, *nfs = nr + *r + 1, fs[N], rt[N];
		memcpy(rt, r + 1, sizeof(agent) * *r);
		sign w, y, z, p[N + 2];

		for (k = 1; k <= min(*f, m); k++) {
			*nr = *r + k;
			memcpy(nr + 1, r + 1, sizeof(agent) * *r);
			memcpy(fs, f + *f - k + 1, sizeof(agent) * k);
			agent nd = vectorsum(fs, k, dr);
			if (d + nd <= MAXDRIVERS) {
				memcpy(nfs, fs, sizeof(agent) * k);
				QSORT(agent, nr + 1, *nr, LTDR);
				nbar(fs, k, r, nr, adj, nf, dr);
				ret += slyce(nr, nf, m - k, adj, d + nd, dr, sp);
			}
			inittwiddle(k, *f, p);
			while (!twiddle(&w, &y, &z, p)) {
				nd = nd - GET(dr, fs[z]) + GET(dr, f[w + 1]);
				fs[z] = f[w + 1];
				if (d + nd <= MAXDRIVERS) {
					memcpy(nr + 1, rt, sizeof(agent) * *r);
					memcpy(nfs, fs, sizeof(agent) * k);
					QSORT(agent, nr + 1, *nr, LTDR);
					nbar(fs, k, r, nr, adj, nf, dr);
					ret += slyce(nr, nf, m - k, adj, d + nd, dr, sp);
				}
			}
		}
	}

	return ret;
}

size_t coalrat(const agent *adj, const chunk *dr, const meter *sp) {

	agent *r = (agent *)malloc(sizeof(agent) * (K + 1) * N);
	agent *f = (agent *)malloc(sizeof(agent) * (N + 1) * N);
        size_t ret = 0;

	for (agent i = 0; i < N; i++) {
		r[0] = 0; f[0] = 1; f[1] = i;
		ret += slyce(r, f, K, adj, 0, dr, sp);
	}

	free(f);
	free(r);
	return ret;
}
