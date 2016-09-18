#ifndef SLYCE_H_
#define SLYCE_H_

__attribute__((always_inline)) inline
void printc(const agent *c, penny v) {

	agent n = *c;
	printf("[ ");
	while (n--) printf("%u ", *(++c));
	printf("] = %.2f£\n", 0.01 * v);
}

size_t coalrat(const agent *adj, const chunk *dr, const meter *sp, IloEnv env, IloFloatVarArray x, IloModel model);

#endif /* SLYCE_H_ */
