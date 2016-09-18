#include "pk.h"

#define MAXLINE 1000

agent readcs(agent *csbuf, chunk *cars) {

	FILE *f = fopen(INPUTFILE, "rt");
	char line[MAXLINE];
	unsigned ret = 0;

	// Skip first 3 lines (N, K, SEED)
	fgets(line, MAXLINE, f);
	fgets(line, MAXLINE, f);
	fgets(line, MAXLINE, f);

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
