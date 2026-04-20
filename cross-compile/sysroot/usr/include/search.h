/*  */
/* @(#)search.h	5.1 4/22/86 */

/*	@(#)search.h	1.1	*/
/* HSEARCH(3C) */
typedef struct entry { char *key, *data; } ENTRY;
typedef enum { FIND, ENTER } ACTION;

/* TSEARCH(3C) */
typedef enum { preorder, postorder, endorder, leaf } VISIT;
