#include "pc.h"

static char line[MAXLINE];

// Reads the graph's adjacency lists

edge readadj(agent *adj, FILE *f) {

	edge ret = 0;

	for (agent i = 0; i < N; i++) {
		fgets(line, MAXLINE, f);
		ret += (adj[i * N] = atoi(strtok(line, " ")));
		for (agent j = 0; j < adj[i * N]; j++)
			adj[i * N + j + 1] = atoi(strtok(NULL, " "));
	}

	return ret / 2;
}

// Reads the solution coalition structure

agent readcs(agent *csbuf, chunk *cars, FILE *f) {

	agent ret = 0;

	while (fgets(line, MAXLINE, f) != NULL) {
		*csbuf = 0;
		char *pch = strtok(line, " ");
		while (pch != NULL) {
			if (*pch == '*') {
				SET(cars, ret);
				pch++;
			}
			csbuf[(*csbuf)++ + 1] = atoi(pch);
			pch = strtok(NULL, " ");
		}
		csbuf += K + 1;
		ret++;
	}

	return ret;
}
