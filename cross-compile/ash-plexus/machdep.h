/*
 * Copyright (C) 1989 by Kenneth Almquist.  All rights reserved.
 * This file is part of ash, which is distributed under the terms specified
 * by the Ash General Public License.  See the file named LICENSE.
 */

/*
 * Most machines require the value returned from malloc to be aligned
 * in some way.  The following macro will get this right on many machines.
 */

#ifndef ALIGN
union align {
      int i;
      char *cp;
};

#define ALIGN(nbytes)	((nbytes) + sizeof(union align) - 1 &~ (sizeof(union align) - 1))
#endif
