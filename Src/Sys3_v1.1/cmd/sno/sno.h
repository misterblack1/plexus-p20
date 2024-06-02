#include <stdio.h>

struct  node {
        struct node *p1;
        struct node *p2;
#ifdef gcos
	int typ;
	int ch;
#else
        char typ;
        char ch;
#endif
};

extern int     freesize;
extern struct  node *lookf;
extern struct  node *looks;
extern struct  node *lookend;
extern struct  node *lookstart;
extern struct  node *lookdef;
extern struct  node *lookret;
extern struct  node *lookfret;
extern int     cfail;
extern int     rfail;
extern struct  node *freelist, *freespace;
extern struct  node *namelist;
extern int     lc;
extern struct  node *schar;

struct node *strst1(), *salloc(), *look(), *copy(), *sgetc(),
	*binstr(), *pop(), *doop(), *push(), *search(),
	*add(), *sub(), *mult(), *div(), *cat(),
	*compile(), *execute(), *eval(), *syspit();
