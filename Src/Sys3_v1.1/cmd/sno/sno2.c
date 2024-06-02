#include "sno.h"

struct node *
compon()
{
	register struct node *a, *b;
	register int c;
	static next;

	if (next == 0)
		schar = sgetc();
	else
		next = 0;
	if (schar == 0) {
		(a=salloc())->typ = 0;
		return (a);
	}
	switch (class (schar->ch)) {

	case 1:
		schar->typ = 5;
		return (schar);

	case 2:
		schar->typ = 16;
		return (schar);

	case 3:
		a = schar;
		for (;;) {
			schar = sgetc();
			if (schar == 0) {
				a->typ = 0;
				return (a);
			}
			if (class (schar->ch) != 3)
				break;
			sfree (schar);
		}
		next = 1;
		a->typ = 7;
		return (a);

	case 4:
		schar->typ = 8;
		return (schar);

	case 5:
		schar->typ = 9;
		return (schar);

	case 6:
		a = schar;
		schar = sgetc();
		if (class (schar->ch) == 3)
			a->typ = 10;
		else
			a->typ = 1;
		next = 1;
		return (a);

	case 7:
		a = schar;
		schar = sgetc();
		if (class (schar->ch) == 3)
			a->typ = 11;
		else
			a->typ = 2;
		next = 1;
		return (a);

	case 8:
		schar->typ = 12;
		return (schar);

	case 9:
		c = schar->ch;
		a = sgetc();
		if (a == 0)
			goto lerr;
		b = schar;
		if (a->ch == c) {
			sfree (schar);
			a->typ = 15;
			a->p1 = 0;
			return (a);
		}
		b->p1 = a;
		for (;;) {
			schar = sgetc();
			if (schar == 0)
			lerr:
				writes ("illegal literal string");
			if (schar->ch == c)
				break;
			a->p1 = schar;
			a = schar;
		}
		b->p2 = a;
		schar->typ = 15;
		schar->p1 = b;
		return (schar);

	case 10:
		schar->typ = 3;
		return (schar);

	case 11:
		schar->typ = 4;
		return (schar);

	}
	b = salloc();
	b->p1 = a = schar;
	schar = sgetc();
	while (schar!=0 && !class (schar->ch)) {
		a->p1 = schar;
		a = schar;
		schar = sgetc();
	}
	b->p2 = a;
	next = 1;
	a = look (b);
	delete (b);
	b = salloc();
	b->typ = 14;
	b->p1 = a;
	return (b);
}

struct node *
nscomp()
{
	register struct node *c;

	while ((c=compon())->typ == 7)
		sfree (c);
	return (c);
}

struct node *
push (stack)
	struct node *stack;
{
	register struct node *a;

	(a=salloc())->p2 = stack;
	return (a);
}

struct node *
pop (stack)
	struct node *stack;
{
	register struct node *a, *s;

	s = stack;
	if (s == 0)
		writes ("pop");
	a = s->p2;
	sfree (s);
	return (a);
}

struct node *
expr (start, eof, e)
	struct node *start, *e;
{
	register struct node *stack, *list, *comp, *space;
	int operand, op, op1;
	struct node *a, *b, *c;
	int d;

	list = salloc();
	e->p2 = list;
	stack = push ((struct node *) NULL);
	stack->typ = eof;
	operand = 0;
	space = start;
l1:
	if (space) {
		comp = space;
		space = 0;
	} else
		comp = compon();

l3:
	op = comp->typ;
	switch (op) {

	case 7:
		space = (struct node *) 1;
		sfree (comp);
		comp = compon();
		goto l3;

	case 10:
		if (space == 0) {
			comp->typ = 1;
			goto l3;
		}

	case 11:
		if (space == 0) {
			comp->typ = 2;
			goto l3;
		}

	case 8:
	case 9:
		if (operand == 0)
			writes ("no operand preceding operator");
		operand = 0;
		goto l5;

	case 14:
	case 15:
		if (operand == 0) {
			operand = 1;
			goto l5;
		}
		if (space == 0)
			goto l7;
		goto l4;

	case 12:
		if (operand == 0)
			goto l5;
		if (space)
			goto l4;
	l7:
		writes ("illegal juxtaposition of operands");

	case 16:
		if (operand == 0)
			goto l5;
		if (space)
			goto l4;
		b = compon();
		op = comp->typ = 13;
		if (b->typ == 5) {
			comp->p1 = 0;
			goto l10;
		}
		comp->p1 = a = salloc();
		b = expr (b, 6, a);
		while ((d=b->typ) == 4) {
			a->p1 = b;
			a = b;
			b = expr ((struct node *) NULL, 6, a);
		}
		if (d != 5)
			writes ("error in function");
		a->p1 = 0;
	l10:
		sfree (b);
		goto l6;

	l4:
		space = comp;
		op = 7;
		operand = 0;
		goto l6;
	}
	if (operand==0)
		writes ("no operand at end of expression");
l5:
	space = 0;
l6:
	op1 = stack->typ;
	if (op > op1) {
		stack = push (stack);
		if (op == 16)
			op = 6;
		stack->typ = op;
		stack->p1 = comp;
		goto l1;
	}
	c = stack->p1;
	stack = pop (stack);
	if (stack == 0) {
		list->typ = 0;
		return (comp);
	}
	if (op1 == 6) {
		if (op != 5)
			writes ("too many ('s");
		goto l1;
	}
	if (op1 == 7)
		c = salloc();
	list->typ = op1;
	list->p2 = c->p1;
	list->p1 = c;
	list = c;
	goto l6;
}

struct node *
match (start, m)
	struct node *start, *m;
{
	register struct node *list, *comp, *term;
	struct node *a;
	int b, bal;

	term = NULL;
	bal = 0;
	list = salloc();
	m->p2 = list;
	comp = start;
	if (!comp)
		comp = compon();
	goto l2;

l3:
	list->p1 = a = salloc();
	list = a;
l2:
	switch (comp->typ) {
	case 7:
		sfree (comp);
		comp = compon();
		goto l2;

	case 12:
	case 14:
	case 15:
	case 16:
		term = 0;
		comp = expr (comp, 6, list);
		list->typ = 1;
		goto l3;

	case 1:
		sfree (comp);
		comp = compon();
		bal = 0;
		if (comp->typ == 16) {
			bal = 1;
			sfree (comp);
			comp = compon();
		}
		a = salloc();
		b = comp->typ;
		if (b == 2 || b == 5 || b == 10 || b == 1)
			a->p1 = 0;
		else {
			comp = expr (comp, 11, a);
			a->p1 = a->p2;
		}
		if (comp->typ != 2) {
			a->p2 = 0;
		} else {
			sfree (comp);
			comp = expr ((struct node *) NULL, 6, a);
		}
		if (bal) {
			if (comp->typ != 5)
				goto merr;
			sfree (comp);
			comp = compon();
		}
		b = comp->typ;
		if (b != 1 && b != 10)
			goto merr;
		list->p2 = a;
		list->typ = 2;
		a->typ = bal;
		sfree (comp);
		comp = compon();
		if (bal)
			term = 0; else
			term = list;
		goto l3;
	}
	if (term)
		term->typ = 3;
	list->typ = 0;
	return (comp);

merr:
	writes ("unrecognized component in match");
	return (NULL);
}

struct node *
compile()
{
	register struct node *b, *comp;
	struct node *m, *r, *l, *xs, *xf, *g, *as;
	struct node *aa;
	register int a;
	int t;

	as = m = l = xs = xf = 0;
	t = 0;
	comp = compon();
	a = comp->typ;
	if (a == 14) {
		l = comp->p1;
		sfree (comp);
		comp = compon();
		a = comp->typ;
	}
	if (a != 7)
		writes ("no space beginning statement");
	sfree (comp);
	if (l == lookdef)
		goto def;
	comp = expr ((struct node *) NULL, 11, r=salloc());
	a = comp->typ;
	if (a == 0)
		goto asmble;
	if (a == 2)
		goto xfer;
	if (a == 3)
		goto assig;
	m = salloc();
	comp = match (comp, m);
	a = comp->typ;
	if (a == 0)
		goto asmble;
	if (a == 2)
		goto xfer;
	if (a == 3)
		goto assig;
	writes ("unrecognized component in match");

assig:
	sfree (comp);
	comp = expr ((struct node *) NULL, 6, as=salloc());
	a = comp->typ;
	if (a == 0)
		goto asmble;
	if (a == 2)
		goto xfer;
	writes ("unrecognized component in assignment");

xfer:
	sfree (comp);
	comp = compon();
	a = comp->typ;
	if (a == 16)
		goto xboth;
	if (a == 0) {
		if (xs!=0 || xf!=0)
			goto asmble;
		goto xerr;
	}
	if (a != 14)
		goto xerr;
	b = comp->p1;
	sfree (comp);
	if (b == looks)
		goto xsuc;
	if (b == lookf)
		goto xfail;

xerr:
	writes ("unrecognized component in goto");

xboth:
	sfree (comp);
	xs = salloc();
	xf = salloc();
	comp = expr ((struct node *) NULL, 6, xs);
	if (comp->typ != 5)
		goto xerr;
	xf->p2 = xs->p2;
	comp = compon();
	if (comp->typ != 0)
		goto xerr;
	goto asmble;

xsuc:
	if (xs)
		goto xerr;
	comp = compon();
	if (comp->typ != 16)
		goto xerr;
	comp = expr ((struct node *) NULL, 6, xs=salloc());
	if (comp->typ != 5)
		goto xerr;
	goto xfer;

xfail:
	if (xf)
		goto xerr;
	comp = compon();
	if (comp->typ != 16)
		goto xerr;
	comp = expr ((struct node *) NULL, 6, xf=salloc());
	if (comp->typ != 5)
		goto xerr;
	goto xfer;

asmble:
	if (l) {
		if (l->typ)
			writes ("name doubly defined");
		l->p2 = comp;
		l->typ = 2; /* type label;*/
	}
	comp->p2 = r;
	if (m) {
		t++;
		r->p1 = m;
		r = m;
	}
	if (as) {
		t += 2;
		r->p1 = as;
		r = as;
	}
	(g=salloc())->p1 = 0;
	if (xs) {
		g->p1 = xs->p2;
		sfree (xs);
	}
	g->p2 = 0;
	if (xf) {
		g->p2 = xf->p2;
		sfree (xf);
	}
	r->p1 = g;
	comp->typ = t;
	comp->ch = lc;
	return (comp);

def:
	r = nscomp();
	if (r->typ != 14)
		goto derr;
	l = r->p1;
	if (l->typ)
		writes ("name doubly defined");
	l->typ = 5; /*type function;*/
	aa = r;
	l->p2 = aa;
	r = nscomp();
	l = r;
	aa->p1 = l;
	if (r->typ == 0)
		goto d4;
	if (r->typ != 16)
		goto derr;

d2:
	r = nscomp();
	if (r->typ != 14)
		goto derr;
	aa->p2 = r;
	r->typ = 0;
	aa = r;
	r = nscomp();
	if (r->typ == 4) {
		sfree (r);
		goto d2;
	}
	if (r->typ != 5)
		goto derr;
	sfree (r);
	if ((r=compon())->typ != 0)
		goto derr;
	sfree (r);

d4:
	r = compile();
	aa->p2 = 0;
	l->p1 = r;
	l->p2 = 0;
	return (r);

derr:
	writes ("illegal component in define");
	return (NULL);
}
