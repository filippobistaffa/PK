#ifndef COAL_H_
#define COAL_H_

// Headers

#include <omp.h>
#include <stdlib.h>
#include "instance.h"
#include "macros.h"
#include "types.h"

#include "iqsort.h"
#include "sorted.h"

#define _C CEILBPC(_N)
#define P(_s, _i) (pm[(_s) * _N + (_i)])
#define BC(_n, _m) (bcm[(_n) * (_N + 1) + (_m)])

typedef int16_t sign;
static size_t bcm[(_N + 1) * (_N + 1)], pm[_N * _N];

template <agent n>
__attribute__((always_inline)) inline
agent *createadj(const edge *g, edge ne, const chunk *l) {

	if (!ne) return NULL;
	agent *adj = (agent *)calloc(n * n, sizeof(agent));
	agent ab[2 * ne];
	edge ei = 0;

	for (agent v1 = 0; v1 < n; v1++)
		for (agent v2 = v1 + 1; v2 < n; v2++) {
			const edge e = g[v1 * n + v2];
			if (e) {
				X(ab, ei) = v1;
				Y(ab, ei) = v2;
				ei++;
			}
		}

	agent *a = ab;

	do {
		adj[a[0] * n + (adj[a[0] * n]++) + 1] = a[1];
		adj[a[1] * n + (adj[a[1] * n]++) + 1] = a[0];
		a += 2;
	} while (--ne);

	for (agent i = 0; i < n; i++)
		QSORT(agent, adj + i * n + 1, adj[i * n], LTL);

	return adj;
}

// Count the number of elements in the buffer set to 1 in the mask

template <typename type>
__attribute__((always_inline)) inline
unsigned maskcount(const type *buf, unsigned n, const chunk *mask) {

	unsigned ret = 0;
	do {
		ret += GET(mask, *buf);
		buf++;
	} while (--n);
	return ret;
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

__attribute__((always_inline)) inline
void neighbours(const agent *f, agent m, const agent *adj, agent *n, const chunk *l) {

	if (m) {
		agent t[_N + 1];
		memcpy(n, adj + *f * _N, sizeof(agent) * (adj[*f * _N] + 1));
		f++;

		while (--m) {
			unionsorted(n + 1, *n, adj + *f * _N + 1, adj[*f * _N], t + 1, t, l);
			memcpy(n, t, sizeof(agent) * (*t + 1));
			f++;
		}
	}
	else *n = 0;
}

__attribute__((always_inline)) inline
void nbar(const agent *f, agent n, const agent *r, const agent *ruf, const agent *adj, agent *nb, const chunk *l) {

	agent a[_N + 1], b[_N + 1];
	neighbours(f, n, adj, a, l);
	agent i = 0;
	while (i < *a && LEL(a + i + 1, ruf + 1)) i++;
	memmove(a + 1, a + i + 1, sizeof(agent) * (*a - i));
	*a -= i;
	neighbours(r + 1, *r, adj, nb, l);
	unionsorted(nb + 1, *nb, ruf + 1, *ruf, b + 1, b, l);
	differencesorted(a + 1, *a, b + 1, *b, nb + 1, nb, l);
}

__attribute__((always_inline)) inline
void li(const agent *f, agent i, agent s, agent *c, const chunk *l) {

	agent t = 0, *o = c;
	i = BC(*f, s) - i + 1;
	*(o++) = s;

	do {
		agent x = 1;
		while (P(s, x) < (size_t)i - t) x++;
		*(o++) = (*f - s + 1) - x + 1;
		if (P(s, x) == (size_t)i - t) {
			while (s-- - 1) { *o = *(o - 1) + 1; o++; }
			break;
		}
		i -= t;
		t = P(s, x - 1);
	} while (--s);

	o = c + 1;
	s = *c;

	do { *o = f[*o]; o++; }
	while (--s);

	QSORT(agent, c + 1, *c, LTL);
}

template <typename type>
void recursive(agent *r, agent *f, agent m, agent maxc, const edge *g, const agent *adj, agent d, const chunk *l, agent maxl,
	       void (*cf)(agent *, agent, const edge *, const agent *, const chunk *, type *), type *data) {

	if (*r && (d || *r == 1))
		cf(r, d, g, adj, l, data);

	if (*f && m) {

		agent k, *nr = r + maxc + 1, *nf = f + _N + 1, *nfs = nr + *r + 1, fs[_N], rt[_N];
		memcpy(rt, r + 1, sizeof(agent) * *r);
		sign w, y, z, p[_N + 2];

		for (k = 1; k <= MIN(*f, m); k++) {
			*nr = *r + k;
			memcpy(nr + 1, r + 1, sizeof(agent) * *r);
			memcpy(fs, f + *f - k + 1, sizeof(agent) * k);
			agent nd = maskcount(fs, k, l);
			if (d + nd <= maxl) {
				memcpy(nfs, fs, sizeof(agent) * k);
				QSORT(agent, nr + 1, *nr, LTL);
				nbar(fs, k, r, nr, adj, nf, l);
				recursive(nr, nf, m - k, maxc, g, adj, d + nd, l, maxl, cf, data);
			}
			inittwiddle(k, *f, p);
			while (!twiddle(&w, &y, &z, p)) {
				nd = nd - GET(l, fs[z]) + GET(l, f[w + 1]);
				fs[z] = f[w + 1];
				if (d + nd <= maxl) {
					memcpy(nr + 1, rt, sizeof(agent) * *r);
					memcpy(nfs, fs, sizeof(agent) * k);
					QSORT(agent, nr + 1, *nr, LTL);
					nbar(fs, k, r, nr, adj, nf, l);
					recursive(nr, nf, m - k, maxc, g, adj, d + nd, l, maxl, cf, data);
				}
			}
		}
	}
}

template <typename type>
void recursivepar(agent id, agent m, agent maxc, const edge *g, const agent *adj, agent d, const chunk *l, agent maxl,
		  void (*cf)(agent *, agent, const edge *, const agent *, const chunk *, type *), type *data) {

	agent *r = (agent *)malloc(sizeof(agent) * (maxc + 1) * _N);
	agent *f = (agent *)malloc(sizeof(agent) * (_N + 1) * _N);
	agent *ft = (agent *)malloc(sizeof(agent) * _N);

	agent a[3] = {1, id, 0};
	cf(a, GET(l, id), g, adj, l, data);

        for (agent i = 0; i < _N; i++) {

                a[1] = i;
                nbar(a + 1, 1, a + 2, a, adj, f, l);

                for (agent k = 1; k <= MIN(*f, m - 1); k++) {
			size_t bc = BC(*f, k);
			agent j = bc * id / _N;
                        while (j < bc * (id + 1) / _N) {
				li(f, j + 1, k, ft, l);
				j++;
				unionsorted(a + 1, 1, ft + 1, *ft, r + 1, r, l);
				nbar(ft + 1, k, a, r, adj, f + _N + 1, l);
				const agent nl = maskcount(r + 1, *r, l);
				if (nl <= maxl) recursive(r, f + _N + 1, m - (k + 1), maxc, g, adj, nl, l, maxl, cf, data);
                        }

                        id = (id + 1) % _N;
                }
        }

	free(ft);
	free(f);
	free(r);
}

__attribute__((always_inline)) inline
void initpar() {

	for (agent i = 0; i <= _N; i++) BC(i, 0) = BC(i, i) = ONE;
	for (agent i = 1; i <= _N; i++) for (agent j = 1; j < i; j++) BC(i, j) = BC(i - 1, j - 1) + BC(i - 1, j);
	for (agent i = 1; i < _N; i++) P(i, 1) = ONE;
	for (agent i = 2; i < _N; i++) P(1, i) = i;
	for (agent i = 2; i < _N; i++) for (agent j = 2; j < _N; j++) P(i, j) = P(i - 1, j) + P(i, j - 1);
}

template <typename type>
void coalitions(const edge *g, void (*cf)(agent *, agent, const edge *, const agent *, const chunk *, type *),
		type *data, agent maxc, const chunk *l, agent maxl) {

	chunk *tl;

	if (!l) {
		tl = (chunk *)malloc(sizeof(chunk) * _C);
		ONES(tl, _N, _C);
	}

	agent *r = (agent *)malloc(sizeof(agent) * (maxc + 1) * _N);
	agent *f = (agent *)malloc(sizeof(agent) * (_N + 1) * _N);
	edge ne = 0;

	for (agent i = 0; i < _N; i++)
		for (agent j = i + 1; j < _N; j++)
			if (g[i * _N + j]) ne++;

	agent *adj = createadj<_N>(g, ne, l ? l : tl);
	edge zero[_N] = {0};

	for (agent i = 0; i < _N; i++)
		if (memcmp(g + i * _N, zero, sizeof(agent) * _N)) {
			r[0] = 0; f[0] = 1; f[1] = i;
			recursive(r, f, maxc, maxc, g, adj, 0, l ? l : tl, maxl, cf, data);
		}

	if (!l) free(tl);
	free(adj);
	free(f);
	free(r);
}

template <typename type>
void parcoalitions(const edge *g, void (*cf)(agent *, agent, const edge *, const agent *, const chunk *, type *),
		   type *data[], agent maxc, const chunk *l, agent maxl) {

	initpar();
	chunk *tl;

	if (!l) {
		tl = (chunk *)malloc(sizeof(chunk) * _C);
		ONES(tl, _N, _C);
	}

	edge ne = 0;

	for (agent i = 0; i < _N; i++)
		for (agent j = i + 1; j < _N; j++)
			if (g[i * _N + j]) ne++;

	agent *adj = createadj<_N>(g, ne, l ? l : tl);
	edge zero[_N] = {0};

	#pragma omp parallel for
	for (agent i = 0; i < _N; i++)
		if (memcmp(g + i * _N, zero, sizeof(agent) * _N))
			recursivepar(i, maxc, maxc, g, adj, 0, l ? l : tl, maxl, cf, data[omp_get_thread_num()]);

	if (!l) free(tl);
	free(adj);
}

#endif /* COAL_H_ */
