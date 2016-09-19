#include "pc.h"

static char line[MAXLINE];

int main(int argc, char *argv[]) {

	agent *csbuf = (agent *)malloc(sizeof(agent) * (K + 1) * N);
	agent *adj = (agent *)malloc(sizeof(agent) * N * N);
	chunk dr[C] = {0};

	// Read input file

	FILE *f = fopen(INPUTFILE, "rt");
	// Skip first 2 lines (N, K)
	fgets(line, MAXLINE, f);
	fgets(line, MAXLINE, f);
	// Read seed
	fgets(line, MAXLINE, f);
	unsigned seed = atoi(line);
	readadj(adj, f);
	agent nc = readcs(csbuf, dr, f);
	fclose(f);

	#ifdef DEBUG
	//puts("Adjacency lists");
	//for (agent i = 0; i < N; i++)
	//	printbuf(adj + i * N + 1, adj[i * N]);
	printf("Solution CS with %u coalitions\n", nc);
	#endif

	meter *sp = createsp(seed);

	#ifdef DEBUG
	for (agent i = 0; i < nc; i++)
		printc(csbuf + i * (K + 1), COALVALUE(csbuf + i * (K + 1), GET(dr, csbuf[i * (K + 1) + 1]), sp));
	puts("");
	#endif

	IloEnv env;
	IloModel model(env);
	IloFloatVarArray x(env, N);
	ostringstream ostr;

	for (agent i = 0; i < N; i++) {
		ostr << "x[" << i << "]";
		x[i] = IloFloatVar(env, 0.0, FLT_MAX, ostr.str().c_str());
		ostr.str("");
	}

	// Coalitional rationality constraints

	#ifdef DEBUG
	puts("Coalitional rationality constraints");
	#endif
	coalrat(adj, dr, sp, env, x, model);

	// Efficiency constraints

	#ifdef DEBUG
	puts("\nEfficiency constraints");
	#endif

	for (agent i = 0; i < nc; i++) {
		IloExpr expr(env);
		for (agent j = 0; j < csbuf[i * (K + 1)]; j++) {
			#ifdef DEBUG
			printf("%sx[%u] ", j ? "+ " : "", csbuf[i * (K + 1) + j + 1]);
			#endif
			expr += x[csbuf[i * (K + 1) + j + 1]];
		}
		#ifdef DEBUG
		printf("== %.2f\n", 0.01 * COALVALUE(csbuf + i * (K + 1), GET(dr, csbuf[i * (K + 1) + 1]), sp));
		#endif
		model.add(expr == 0.01 * COALVALUE(csbuf + i * (K + 1), GET(dr, csbuf[i * (K + 1) + 1]), sp));
		expr.end();
	}

	#ifdef DEBUG
	puts("");
	#endif

	IloCplex cplex(model);
	IloTimer timer(env);
	timer.start();

	#ifndef DEBUG
	cplex.setOut(env.getNullStream());
	#endif

	if (!cplex.solve()) {
		env.out() << "Core is empty" << endl;
		exit(1);
	}

	timer.stop();
	IloNumArray xval(env);
	#ifdef DEBUG
	env.out() << "Solution status = " << cplex.getStatus() << endl;
	env.out() << "Elapsed time = " << timer.getTime() << endl;
	#endif
	cplex.getValues(xval, x);
	env.out() << "Payments = " << xval << endl;
	env.end();

	free(csbuf);
	free(adj);
	free(sp);

	return 0;
}
