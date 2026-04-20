/*
 * Some versions of errno.h don't declare errno, so we do it ourself.
 *
 * Copyright (C) 1989 by Kenneth Almquist.  All rights reserved.
 * This file is part of ash, which is distributed under the terms specified
 * by the Ash General Public License.  See the file named LICENSE.
 */

#include <sys/errno.h>

extern int errno;
