#ifndef IO_H_
#define IO_H_

#define MAXLINE 1000

// Prints the content given buffer

#include <iostream>
template <typename type>
__attribute__((always_inline)) inline
void printbuf(const type *buf, unsigned n, const char *name = NULL) {

	if (name) printf("%s = [ ", name);
	else printf("[ ");
	while (n--) std::cout << *(buf++) << " ";
	printf("]\n");
}

// Reads the graph's adjacency lists

edge readadj(agent *adj, FILE *f);

// Reads the solution coalition structure

agent readcs(agent *csbuf, chunk *cars, FILE *f);

#endif /* IO_H_ */
