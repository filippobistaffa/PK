#ifndef IO_H_
#define IO_H_

#include <iostream>
template <typename type>
__attribute__((always_inline)) inline
void printbuf(const type *buf, unsigned n, const char *name = NULL) {

	if (name) printf("%s = [ ", name);
	else printf("[ ");
	while (n--) std::cout << *(buf++) << " ";
	printf("]\n");
}

agent readcs(agent *csbuf);

#endif /* IO_H_ */
