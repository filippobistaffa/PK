#ifndef PK_H_
#define PK_H_

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/time.h>
#include <immintrin.h>

#include "instance.h"
#include "params.h"
#include "macros.h"
#include "types.h"

// Computes the value of a coalition C (assumes only one driver per coalition)
// C = Coalition ( C[0] = Cardinality, C[1] ... C[C[0]] = Elements )
// D = Car or not?
// SP = Shortest path matrix

#define COALVALUE(C, D, SP) ((D) ? (PATHCOST(minpath((C) + 1, *(C), 1, SP)) + CARCOST) : TICKETCOST)

#define PATHCOST(p) ROUND(penny, (float)(p) / METERSPERLITRE * PENNYPERLITRE)
#define DIST(dx, dy) (sqrt((dx) * (dx) + (dy) * (dy)))
#define ROUND(type, i) ((type)(i))

#define R5 2520
#define R4 90
#define R3 6

#define C CEILBPC(N)

#include "io.h"
#include "sp.h"

#endif /* PK_H_ */
