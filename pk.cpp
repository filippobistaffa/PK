#include "pk.h"

static char line[MAXLINE];

typedef struct {
	value *sm, *x; 		// surplus matrix, payoff vector
	const agent *csbuf;	// solution coalition structure
	const agent *rev;	// rev[i] = j => i is in the j-th coalition in csbuf
	size_t ccount[N];	// ccount[i] = number of coalitions containing i
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

		pkd->ccount[*c]++;
		// nc = C' - C
		agent nc[N];
		differencesorted(pkd->csbuf + pkd->rev[*c] * (K + 1) + 1, pkd->csbuf[pkd->rev[*c] * (K + 1)],
				 oc + 1, *oc, nc + 1, nc, l);
		j = *nc;
		t = nc + 1;

		while (j--) {
			pkd->sm[*c * N + *t] = MAX(pkd->sm[*c * N + *t], -vmx);
			t++;
		}

		c++;

	} while (--i);
}

int main(int argc, char *argv[]) {

	agent *csbuf = (agent *)malloc(sizeof(agent) * (K + 1) * N);
	agent *adj = (agent *)malloc(sizeof(agent) * N * N);
	chunk l[C] = {0};

	// Read input file

	FILE *f = fopen(INPUTFILE, "rt");
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

	for (agent i = 0; i < nc; i++) {
		const value val = srvalue(csbuf + i * (K + 1), maskcount(csbuf + i * (K + 1) + 1, csbuf[i * (K + 1)], l), sp);
		#ifndef CSV
		printbuf(csbuf + i * (K + 1) + 1, csbuf[i * (K + 1)], NULL, NULL, " = ");
		printf("%.2f\n", val);
		#endif
	}

	free(csbuf);
	free(adj);
	free(sp);

	return 0;
}
