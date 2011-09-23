/* Placed into the public domain */
#ifndef CCAN_TCON_H
#define CCAN_TCON_H
#include "config.h"

/**
 * TCON - declare a _tcon type containing canary variables.
 * @decls: the semi-colon separated list of type canaries.
 *
 * This declares a _tcon member for a structure.  It should be the
 * last element in your structure; with sufficient compiler support it
 * will not use any actual storage.  tcon_to_raw() will compare
 * expressions with one of these "type canaries" to cause warnings if
 * the container is misused.
 *
 * A type of "void *" will allow tcon_to_raw() to pass on any (pointer) type.
 *
 * Example:
 *	// Simply typesafe linked list.
 *	struct list_head {
 *		struct list_head *prev, *next;
 *	};
 *
 *	struct string_list {
 *		struct list_head raw;
 *		TCON(char *canary);
 *	};
 *
 *	// More complex: mapping from one type to another.
 *	struct map {
 *		void *contents;
 *	};
 *
 *	struct int_to_string_map {
 *		struct map raw;
 *		TCON(char *charp_canary; int int_canary);
 *	};
 */
#if HAVE_FLEXIBLE_ARRAY_MEMBER
#define TCON(decls) struct { decls; } _tcon[]
#else
#define TCON(decls) struct { decls; } _tcon[1]
#endif

/**
 * tcon_check - typecheck a typed container
 * @x: the structure containing the TCON.
 * @canary: which canary to check against.
 * @expr: the expression whose type must match the TCON (not evaluated)
 *
 * This macro is used to check that the expression is the type
 * expected for this structure (note the "useless" sizeof() argument
 * which contains this comparison with the type canary).
 *
 * It evaluates to @x so you can chain it.
 *
 * Example:
 *	#define tlist_add(h, n, member) \
 *		list_add(&tcon_check((h), canary, (n))->raw, &(n)->member)
 */
#define tcon_check(x, canary, expr)				\
	(sizeof((x)->_tcon[0].canary == (expr)) ? (x) : (x))


/**
 * tcon_cast - cast to a canary type for this container (or void *)
 * @x: a structure containing the TCON.
 * @canary: which canary to cast to.
 * @expr: the value to cast
 *
 * This is used to cast to the correct type for this container.  If the
 * platform doesn't HAVE_TYPEOF, then it casts to void * (which will
 * cause a warning if the user doesn't expect a pointer type).
 */
#if HAVE_TYPEOF
#define tcon_cast(x, canary, expr) ((__typeof__((x)->_tcon[0].canary))(expr))
#else
#define tcon_cast(x, canary, expr) ((void *)(expr))
#endif

#endif /* CCAN_TCON_H */