*
* The following devices are those that can be specified in the system
* description file.  The name specified must agree with the name shown.
*
da11b	4	36	7	da	8	0	11	1	5	da
dh11	8	77	6	dh	16	0	1	16	5	tty
dz11	8	77	6	dz	8	0	1	8	5	tty
dza	8	77	6	dza	8	0	1	8	5	tty
dzb	8	77	46	dzb	8	0	1	8	5	tty
dm11	4	0	2	dm	4	0	0	16	4	inter
rp03	4	76	15	rp	24	0	7	8	5
rp04	4	76	15	hp	64	0	7	8	5
rk05	4	76	15	rk	14	0	7	8	5
rl01	4	76	15	rl	8	0	7	4	5
tu10	4	36	15	tm	12	1	6	4	5
tu16	4	36	15	ht	32	1	6	4	5
dr11c	8	32	6	cat	8	0	8	1	5
du11	8	36	6	du	8	0	3	1	5	du
lp11	4	32	5	lp	4	0	14	1	4
dn11	4	32	6	dn	8	0	4	4	5
rf11	4	6	15	rf	16	3	12	8	5
rs04	4	36	15	hs	32	2	12	8	5
dl11	8	77	6	kl	8	0	0	1	5	tty
vp	4	33	5	vp	16	0	14	1	5	vp
kmc11	8	37	6	kmc	8	0	18	1	5
dmc11	8	36	6	dmc	8	0	19	1	5
dmb	8	37	6	dmb	8	0	19	1	5
dmr	8	37	6	dmr	8	0	19	1	5
vpm	0	37	206	vpm	0	0	15	16	5
trace	0	35	6	tr	0	0	16	1	1
*
* PWB devices
*
dqs11b	4	36	7	dqs	8	0	5	1	5
prf	0	7	204	prf	0	0	25	1	0
st	0	37	204	st	0	0	30	50	0	st
pcl11b	8	37	7	pcl	32	0	33	8	5	pcl
pclctrl	0	34	204	pclc	0	0	34	1	0
*
* The following three devices must not be specified in the system description
* file.  They are here to supply information to the config program.
*
memory	0	6	324	mm	0	0	2	1	0
tty	0	27	324	sy	0	0	13	1	0
errlog	0	34	324	err	0	0	20	1	0
$$$
*
* The following entries form the alias table.
*
rp11	rp03
la36	dl11
kl11	dl11
tm11	tu10
rp05	rp04
rp06	rp04
rl11	rl01
rs03	rs04
te16	tu16
tu45	tu16
dqs11a	dqs11b
$$$
*
* The following entries form the tunable parameter table.
*
buffers	NBUF
inodes	NINODE
files	NFILE
mounts	NMOUNT
coremap	CMAPSIZ
swapmap	SMAPSIZ
calls	NCALL
procs	NPROC
texts	NTEXT
clists	NCLIST
sabufs	NSABUF	8
power	POWER	0
maxproc	MAXUP	25
* hashbuf must be a power of 2
hashbuf	NHBUF	64
