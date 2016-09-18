#ifndef VALUE_H_
#define VALUE_H_

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

meter minpath(agent *c, agent n, agent dr, const meter *sp);

#endif /* VALUE_H_ */
