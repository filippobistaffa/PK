#include "pk.h"

static char line[MAXLINE];

typedef struct {
	value *sm;		// surplus matrix
	const value *x; 	// payoff vector
	const agent *csbuf;	// solution coalition structure
	const agent *rev;	// rev[i] = j => i is in the j-th coalition in csbuf
	size_t *nc;		// nc[i] = number of coalitions containing i
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

	// Excess of coalition
	register value vmx = vc + vectorsum(c + 1, *c, pkd->x);

	if ((i = *(c++))) do {

		pkd->nc[*c]++;
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

__attribute__((always_inline)) inline
void creatematrix(value *sm, size_t *nc, const value *x, const edge *g, const agent *csbuf, const agent *rev, const chunk *l,
		  const meter *sp) {

	memset(nc, 0, sizeof(size_t) * N);

	for (agent i = 0; i < N; i++)
		for (agent j = i; j < N; j++)
			sm[i * N + j] = sm[j * N + i] = -INFINITY;

	pkdata pkd = { .sm = sm, .x = x, .csbuf = csbuf, .rev = rev, .nc = nc, .sp = sp };
	coalitions(g, updatesm, &pkd, K, l, 1);
}

agent computekernel(value *x, value epsilon, const agent *csbuf, agent nc, const chunk *l, const agent *deg, const meter *sp) {

	agent *rev = (agent *)malloc(sizeof(agent) * N);

	for (agent i = 0; i < nc; ++i)
		for (agent j = 0; j < csbuf[i * (K + 1)]; ++j)
			rev[csbuf[i * (K + 1) + j + 1]] = i;

	printbuf(rev, N, "rev");

	value *sm = (value *)malloc(sizeof(value) * N * N);

	free(rev);
	free(sm);

	return 0;
}

int main(int argc, char *argv[]) {

	agent *csbuf = (agent *)malloc(sizeof(agent) * (K + 1) * N);
	agent *adj = (agent *)malloc(sizeof(agent) * N * N);
	chunk l[C] = {0};

	// Read input file

	FILE *f = fopen(argv[1], "rt");
	// Skip first 2 lines (N, K)
	fgets(line, MAXLINE, f);
	fgets(line, MAXLINE, f);
	// Read seed
	fgets(line, MAXLINE, f);
	unsigned seed = atoi(line);
	edge ne = readadj(adj, f);
	agent nc = readcs(f, csbuf, l);
	fclose(f);

	printf("%u edges\n", ne);

	for (agent i = 0; i < N; i++)
		printbuf(adj + i * N + 1, adj[i * N]);

	printf("%u coalitions\n", nc);
	meter *sp = createsp(seed);

	// payoff vector
	value *x = (value *)malloc(sizeof(value) * N);

	for (agent i = 0; i < nc; i++) {
		QSORT(agent, csbuf + i * (K + 1) + 1, csbuf[i * (K + 1)], LTL);
		const value val = srvalue(csbuf + i * (K + 1), maskcount(csbuf + i * (K + 1) + 1, csbuf[i * (K + 1)], l), sp);
		for (agent j = 0; j < csbuf[i * (K + 1)]; ++j)
                        x[csbuf[i * (K + 1) + j + 1]] = -val / csbuf[i * (K + 1)];
		#ifndef CSV
		printbuf(csbuf + i * (K + 1) + 1, csbuf[i * (K + 1)], NULL, NULL, " = ");
		printf("%.2f\n", val);
		#endif
	}

	computekernel(x, EPSILON, csbuf, nc, l, NULL, sp);
	printbuf(x, N, "x");

	free(csbuf);
	free(adj);
	free(sp);
	free(x);

	return 0;
}
