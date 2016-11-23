#include "pk.h"

static char line[MAXLINE];

typedef struct {
	value *sm;		// surplus matrix
	const value *x; 	// payoff vector
	const agent *csbuf;	// solution coalition structure
	const agent *rev;	// rev[i] = j => i is in the j-th coalition in csbuf
	size_t *cc;		// cc[i] = number of coalitions containing i
	const meter *sp;	// shortest paths matrix
} pkdata;

template <typename type>
__attribute__((always_inline)) inline
type vectorsum(const agent *r, agent n, const type *x) {

	register type ret = 0;
	do ret += x[*(r++)];
	while (--n);
	return ret;
}

template <typename type>
void updatesm(agent *c, agent nl, const edge *g, const agent *adj, const chunk *l, type *data) {


	pkdata *pkd = (pkdata *)data;
	register agent i, j, *t, *oc = c;

	// Value of current coalition
	register value vc = srvalue(c, nl, pkd->sp);

	//printbuf(c + 1, *c, NULL, NULL, " = ");
	//printf("%.2f\n", vc);

	// Excess of coalition
	register value vmx = vc + vectorsum(c + 1, *c, pkd->x);
	//printbuf(c + 1, *c, NULL, NULL, " = ");
	//printf("%.2f\n", vmx);

	if ((i = *(c++))) do {

		pkd->cc[*c]++;
		// dc = C' - C
		agent dc[N];
		differencesorted(pkd->csbuf + pkd->rev[*c] * (K + 1) + 1, pkd->csbuf[pkd->rev[*c] * (K + 1)],
				 oc + 1, *oc, dc + 1, dc, l);
		j = *dc;
		t = dc + 1;

		while (j--) {
			pkd->sm[*c * N + *t] = MAX(pkd->sm[*c * N + *t], -vmx);
			t++;
		}

		c++;

	} while (--i);
}

#include <algorithm>

__attribute__((always_inline)) inline
void creatematrix(value *sm, size_t *cc, const value *x, const edge *g, const agent *csbuf, const agent *rev, const chunk *l,
		  const meter *sp) {

	memset(cc, 0, sizeof(size_t) * N);
	std::fill_n(sm, N * N, -INFINITY);
	pkdata pkd = { .sm = sm, .x = x, .csbuf = csbuf, .rev = rev, .cc = cc, .sp = sp };
	coalitions(g, updatesm, &pkd, K, l, 1);
}

void computekernel(value *x, value epsilon, const edge *g, const agent *csbuf, agent nc, value val, const chunk *l,
		    const agent *deg, const meter *sp) {

	agent *rev = (agent *)malloc(sizeof(agent) * N);

	for (agent i = 0; i < nc; ++i)
		for (agent j = 0; j < csbuf[i * (K + 1)]; ++j)
			rev[csbuf[i * (K + 1) + j + 1]] = i;

	//printbuf(rev, N, "rev");
	value *sing = (value *)malloc(sizeof(value) * N);

	for (agent i = 0; i < N; ++i) {
		agent csing[] = { 1, i };
		sing[i] = srvalue(csing, GET(l, i), sp);
	}

	//printbuf(sing, N, "sing");
	value *sm = (value *)malloc(sizeof(value) * N * N);
	size_t *cc = (size_t *)malloc(sizeof(size_t) * N);
	//size_t it = 0;
	value d;

	do {
		//it++;
		creatematrix(sm, cc, x, g, csbuf, rev, l, sp);
		printf("CRC32 = %u\n", crc32(sm, sizeof(value) * N * N));
		d = -INFINITY;
		agent mi, mj;

		for (agent i = 0; i < N; i++)
			for (agent j = i + 1; j < N; j++) {
				value t;
				if (sm[i * N + j] == -INFINITY && sm[j * N + i] == -INFINITY) t = -INFINITY;
				else t = sm[i * N + j] - sm[j * N + i];
				if (t > d) { d = t; mi = i; mj = j; }
			}

		value e = MAX(x[mj] + sing[mj], MIND);
		if (e >= d / 2) e = d / 2;
		x[mi] += e;
		x[mj] -= e;

	} while (d / val > epsilon);

	free(sing);
	free(rev);
	free(sm);
	free(cc);
}

int main(int argc, char *argv[]) {

	// Read input file
	FILE *f = fopen(argv[1], "rt");
	// Skip first 2 lines (N, K)
	fgets(line, MAXLINE, f);
	fgets(line, MAXLINE, f);
	// Read seed
	fgets(line, MAXLINE, f);
	unsigned seed = atoi(line);

	edge *g = (edge *)malloc(sizeof(edge) * N * N);
	readg(g, f);

	agent *csbuf = (agent *)malloc(sizeof(agent) * (K + 1) * N);
	chunk l[C] = {0};
	agent nc = readcs(csbuf, l, f);

	fclose(f);

	puts("Adjacency matrix");
	for (agent i = 0; i < N; i++)
		printbuf(g + i * N, N, NULL, "% 2u");
	puts("");

	printf("%u coalitions\n", nc);
	meter *sp = createsp(seed);

	// payoff vector
	value *x = (value *)malloc(sizeof(value) * N);

	// solution value
	value totval = 0;

	for (agent i = 0; i < nc; i++) {
		QSORT(agent, csbuf + i * (K + 1) + 1, csbuf[i * (K + 1)], LTL);
		const value val = srvalue(csbuf + i * (K + 1), maskcount(csbuf + i * (K + 1) + 1, csbuf[i * (K + 1)], l), sp);
		totval += val;
		for (agent j = 0; j < csbuf[i * (K + 1)]; ++j)
                        x[csbuf[i * (K + 1) + j + 1]] = -val / csbuf[i * (K + 1)];
		#ifndef CSV
		printbuf(csbuf + i * (K + 1) + 1, csbuf[i * (K + 1)], NULL, NULL, " = ");
		printf("%.2f\n", val);
		#endif
	}

	struct timeval t1, t2;
	gettimeofday(&t1, NULL);
	computekernel(x, EPSILON, g, csbuf, nc, totval, l, NULL, sp);
	gettimeofday(&t2, NULL);
	printbuf(x, N, "\nPayoff vector", "%.2f");
	printf("Elapsed time = %f\n", (double)(t2.tv_usec - t1.tv_usec) / 1e6 + t2.tv_sec - t1.tv_sec);

	free(csbuf);
	free(sp);
	free(x);
	free(g);

	return 0;
}
