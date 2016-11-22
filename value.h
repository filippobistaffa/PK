#ifndef VALUE_H_
#define VALUE_H_

// Headers

#include <limits.h>
#include <immintrin.h>

#include "instance.h"
#include "macros.h"
#include "types.h"

// Cost parameters

#define CARCOST 100
#define TICKETCOST 300
#define CENTSPERLITRE 130
#define METERSPERLITRE 15000

#define R5 2520
#define R4 90
#define R3 6

// Computes the value of a coalition C (assumes only one driver per coalition)

value srvalue(agent *c, agent nl, void *sp);

#endif /* VALUE_H_ */
