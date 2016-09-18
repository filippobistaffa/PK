#ifndef SP_H_
#define SP_H_

#include <math.h>

#define IDX "dat/idx.dat"
#define ADJ "dat/adj.dat"
#define XY "dat/xy.dat"
#define SS "dat/ss.dat"

typedef struct { place p; dist f; } item;

meter *createsp();

#endif /* SP_H_ */
