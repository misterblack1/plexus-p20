/*
 * Copyright (C) 1989 by Kenneth Almquist.  All rights reserved.
 * This file is part of ash, which is distributed under the terms specified
 * by the Ash General Public License.  See the file named LICENSE.
 *
 * The following defines avoid global name conflicts for linkers that
 * only look at the first six characters.
 */

#define builtinfunc bltfu
#define builtinloc bltlo
#define cmdlookup cmdlk
#define command cmd_
#define commandtext cmdtx
#define delete_cmd_entry delce
#define environment envmt
#define expandarg exarg
#define expandhere exher
#define expandmeta exmet
#define growstackblock grosb
#define heredoclist herdl
#define lookupvar lookv
#define match_begin matcb
#define number_parens numpa
#define parsebackquote parbq
#define parsefile parfi
#define parsenextc parnx
#define parsenleft parnl
#define pushednleft pusnl
#define pushedstring pusst
#define readtoken1 rtok1
#define setinputfd stifd
#define setinputfile stifi
#define setinteractive stint
#define setvareq stveq
#define stacknleft stknl
