/* defines for lint message buffering scheme 
 * be sure to include lerror.h before lmanifest
 */

/* number of chars in NAME, and filename */
# define LCHNM 8
# define LFNM 14

#define	NUMBUF	22
#define	MAXBUF	100

# define PLAINTY	0
# define STRINGTY	01
# define DBLSTRTY	02
# define CHARTY		04
# define NUMTY		010

# define SIMPL		020
# define WERRTY		0100
# define UERRTY		0

# define TMPDIR	"/usr/tmp"
# define TMPLEN	sizeof( TMPDIR )

# define NOTHING	0
# define ERRMSG	01
# define FATAL		02
# define CCLOSE		04
# define HCLOSE		010

struct hdritem {
	char	hname[ LFNM ];
	char	sname[ LFNM ];
	int	hcount;
};

# define HDRITEM	struct hdritem
# define NUMHDRS	100

struct crecord {
    int	code;
    int	lineno;
    union {
	char	name1[LCHNM];
	char	char1;
	int	number;
    } arg1;
    char	name2[LCHNM];
};

# define CRECORD	struct crecord
# define CRECSZ		sizeof ( CRECORD )

# define OKFSEEK	0
# define PERMSG		((long) CRECSZ * MAXBUF )

struct hrecord {
    int		msgndx;
    int		code;
    int		lineno;
    union {
	char	name1[ LCHNM ];
	char	char1;
	int	number;
    } arg1;
    char	name2[ LCHNM ];
};

# define HRECORD	struct hrecord
# define HRECSZ		sizeof( HRECORD )

enum boolean { true, false };

/* for pass2 in particular */

# define NUM2MSGS	13
# define MAX2BUF	100

struct c2record {
    char	name[ LCHNM ];
    int		number;
    int		file1;
    int		line1;
    int		file2;
    int		line2;
};

# define C2RECORD	struct c2record
# define C2RECSZ	sizeof( C2RECORD )
# define PERC2SZ	((long) C2RECSZ * MAX2BUF )

# define NMONLY	1
# define NMFNLN	2
# define NM2FNLN	3
# define ND2FNLN	4
