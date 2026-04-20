/* SID */
/* @(#)bar.h	5.1 4/22/86 */

#define	BARMAG	"!<arch>\n"
#define	SARMAG	8

#define	ARFMAG	"`\n"

struct bar_hdr {
	char	ar_name[16];
	char	ar_date[12];
	char	ar_uid[6];
	char	ar_gid[6];
	char	ar_mode[8];
	char	ar_size[10];
	char	ar_fmag[2];
};
