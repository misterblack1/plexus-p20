#define NDQ 6
#define NBF 2

struct dqsbuf	{
	int bufl,bufc;
	char *bufb;
	struct dqsbuf *bufn;
	struct buf *bufa;
};

struct dqsdat	{
	int  state;
	char qcase,open;
	 struct device *addr;
	int  dly;
	int  ttd,ack0,wack,ackx;
	char eot,enq,nak,etb,etx;
	char slp,time,try;
	int  cc,resp,uoff;
	struct dqsbuf *x,*q,*u;
	struct dqsbuf bf[NBF];
};

#ifndef DQSFUDGE
struct dqsdat dqsx[] {
#ifdef DQS11B_0
	{ 0,0,0,0,0,026402,070020,064420,0150440,067,055,075,046,03 },
#endif
#ifdef DQS11A_0
	{ 0,0,0,0,2,0102402,0130020,035420,0160440,04,0205,025,0227,0203 },
#endif
#ifdef DQS11B_1
	{ 0,0,0,0,0,026402,070020,064420,0150440,067,055,075,046,03 },
#endif
#ifdef DQS11A_1
	{ 0,0,0,0,2,0102402,0130020,035420,0160440,04,0205,025,0227,0203 },
#endif
#ifdef DQS11B_2
	{ 0,0,0,0,0,026402,070020,064420,0150440,067,055,075,046,03 },
#endif
#ifdef DQS11A_2
	{ 0,0,0,0,2,0102402,0130020,035420,0160440,04,0205,025,0227,0203 },
#endif
#ifdef DQS11B_3
	{ 0,0,0,0,0,026402,070020,064420,0150440,067,055,075,046,03 },
#endif
#ifdef DQS11A_3
	{ 0,0,0,0,2,0102402,0130020,035420,0160440,04,0205,025,0227,0203 },
#endif
#ifdef DQS11B_4
	{ 0,0,0,0,0,026402,070020,064420,0150440,067,055,075,046,03 },
#endif
#ifdef DQS11A_4
	{ 0,0,0,0,2,0102402,0130020,035420,0160440,04,0205,025,0227,0203 },
#endif
#ifdef DQS11B_5
	{ 0,0,0,0,0,026402,070020,064420,0150440,067,055,075,046,03 },
#endif
#ifdef DQS11A_5
	{ 0,0,0,0,2,0102402,0130020,035420,0160440,04,0205,025,0227,0203 },
#endif
};
#endif
