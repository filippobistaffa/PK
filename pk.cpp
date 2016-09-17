#include "pk.h"

int main(int argc, char *argv[]) {

	agent *csbuf = (agent *)malloc(sizeof(agent) * (K + 1) * N);
	agent nc = readcs(csbuf);
	printf("%u coalitions\n", nc);

	for (agent i = 0; i < nc; i++)
		printbuf(csbuf + i * (K + 1) + 1, csbuf[i * (K + 1)]);

	free(csbuf);

	return 0;
}
