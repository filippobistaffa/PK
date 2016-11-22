#ifndef IO_H_
#define IO_H_

#define MAXLINE 1000

// Prints the content given buffer

#include <stdio.h>
#include <iostream>
template <typename type>
__attribute__((always_inline)) inline
void printbuf(const type *buf, unsigned n, const char *name = NULL, const char *format = NULL, const char *after = NULL) {

	if (name) printf("%s = [ ", name);
	else printf("[ ");
	while (n--) {
		if (format) { printf(format, *(buf++)); printf(" "); }
		else std::cout << *(buf++) << " ";
	}
	printf("]%s", (after) ? after : "\n");
}

// Reads the graph's adjacency lists

edge readadj(agent *adj, FILE *f);

// Reads the solution coalition structure

agent readcs(FILE *f, agent *csbuf, chunk *cars);

#endif /* IO_H_ */
