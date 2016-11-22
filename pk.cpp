#include "pk.h"

static char line[MAXLINE];

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
