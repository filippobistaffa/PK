#ifndef SORTED_H_
#define SORTED_H_

#include <string.h>

/*
 * x = first input set
 * m = cardinality of x
 * y = second input set
 * n = cardinality of y
 * z = output set
 * o = cardinality of z
 * l = leaders bitmask
 */

template <typename type, typename out> __attribute__((always_inline)) inline
void unionsorted(const type *x, unsigned m, const type *y, unsigned n, type *z, out *o, const chunk *l) {

	*o = 0;

	while (m && n) {
		if (LTL(x, y)) { *(z++) = *(x++); m--; }
		else if (LTL(y, x)) { *(z++) = *(y++); n--; }
		else { *(z++) = *(y++); x++; m--; n--; }
		(*o)++;
	}

	(*o) += m + n;
	if (m) memcpy(z, x, sizeof(type) * m);
	else memcpy(z, y, sizeof(type) * n);
}

#define CONTAINSORDERED(X, V, L) (binarysearch(X, (V) + 1, *(V), L) < *(V))

template <typename type> __attribute__((always_inline)) inline
unsigned binarysearch(type x, const type *buf, unsigned n, const chunk *l) {

	if (n) {
		#define MIDPOINT(_min, _max) (_min + ((_max - _min) / 2))
		unsigned imin = 0, imid, imax = n - 1;

		while (imin < imax) {
			imid = MIDPOINT(imin, imax);
			if (LTL(buf + imid, &x)) imin = imid + 1;
			else imax = imid;
		}

		if (imax == imin && buf[imin] == x) return imin;
	}
	return n + 1;
}

template <typename type, typename out> __attribute__((always_inline)) inline
void differencesorted(const type *x, unsigned m, const type *y, unsigned n, type *z, out *o, const chunk *l) {

	*o = 0;

	while (m && n) {
		if (LTL(x, y)) { *(z++) = *(x++); m--; (*o)++; }
		else if (LTL(y, x)) { y++; n--; }
		else { y++; x++; m--; n--; }
	}

	if (!m) return;
	(*o) += m;
	memcpy(z, x, sizeof(type) * m);
}

template <typename type, typename out> __attribute__((always_inline)) inline
void removesorted(type x, const type *buf, unsigned n, type *z, out *o, const chunk *l) {

	const unsigned i = binarysearch(x, buf, n, l);
	if (i < n) {
		memcpy(z, buf, sizeof(type) * i);
		memcpy(z + i, buf + i + 1, sizeof(agent) * (n - i - 1));
		*o = n - 1;
	} else memcpy(z, buf, sizeof(type) * n);
}

#endif /* SORTED_H_ */
