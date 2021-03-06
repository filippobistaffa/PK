#include "pk.h"

static char line[MAXLINE];

// Reads the graph's adjacency matrix

void readg(edge *g, FILE *f) {

	for (agent i = 0; i < N; i++) {
		fgets(line, MAXLINE, f);
		g[i * N] = atoi(strtok(line, " "));
		for (agent j = 1; j < N; j++)
			g[i * N + j] = atoi(strtok(NULL, " "));
	}
}

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

agent readcs(agent *csbuf, chunk *dr, FILE *f) {

	agent ret = 0;

	while (fgets(line, MAXLINE, f) != NULL) {
		*csbuf = 0;
		char *pch = strtok(line, " ");
		while (pch != NULL) {
			bool isdr = false;
			if (*pch == '*') {
				isdr = true;
				pch++;
			}
			const agent i = atoi(pch);
			csbuf[(*csbuf)++ + 1] = i;
			if (isdr) SET(dr, i);
			pch = strtok(NULL, " ");
		}
		csbuf += K + 1;
		ret++;
	}

	return ret;
}
