/*
 * Copyright (C) 1989 by Kenneth Almquist.  All rights reserved.
 * This file is part of ash, which is distributed under the terms specified
 * by the Ash General Public License.  See the file named LICENSE.
 */

#ifdef __STDC__
void init(void);
void reset(void);
void initshellproc(void);
#else
void init();
void reset();
void initshellproc();
#endif
