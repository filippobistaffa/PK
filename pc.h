#ifndef PC_H_
#define PC_H_

#include <omp.h>
#include <stdio.h>
#include <float.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/time.h>
#include <immintrin.h>

// Cplex headers
#include <ilcplex/ilocplex.h>

#include "instance.h"
#include "params.h"
#include "macros.h"
#include "types.h"

#define C CEILBPC(N)

using namespace std;

#include "iqsort.h"
#include "slyce.h"
#include "value.h"
#include "io.h"
#include "sp.h"

#endif /* PC_H_ */
