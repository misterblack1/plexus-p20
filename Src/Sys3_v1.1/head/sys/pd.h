#define PDLOGVOL	16
#define PDUNITS		4
#define IDINT (0x80 << 1)

#define SSEEK 1
#define SIO 2
#define SRECAL 3

struct initinfo pd_init[PDUNITS];

struct diskconf pd_sizes[PDLOGVOL*PDUNITS];
