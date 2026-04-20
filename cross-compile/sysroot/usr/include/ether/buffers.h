/* SID @(#)buffers.h	5.1 */
/*  */
/* @(#)buffers.h	1.3 5/1/83 */



		/* INTERLAN Ethernet Data Formats */

		/* Transmitter buffer structure */

struct ettdb {
	char t_dest[6];			/* destination address */
	int  t_proto;			/* protocol field */

	int  t_type;			/* type field */
	int  t_seq;			/* sequence field */
	int  t_size;			/* # bytes in data field */
	char t_data[TBSIZE];		/* transmitted data */
};

		/* Received buffer structure */
		/* r_status, r_dest, and r_src are byte swapped */

struct etrdb {
	char r_dum0;			/* 0 (constant) */
	char r_status;			/* receive frame status */
	int  r_length;			/* frame length */
	char r_dest[6];			/* destination address of packet */
	char r_src[6];			/* source address of packet */
	int  r_proto;			/* protocol field */

	int  r_type;			/* type field */
	int  r_seq;			/* sequence field */
	int  r_size;			/* # bytes in data field */
	char r_data[RBSIZE];		/* received data */
	char r_crc[4];			/* crc of data */
};

	/* status block from reset and "report statistics" commands */

struct sb {
	int  null0;		/* 00-01 ZERO */
	int  framelength;	/* 02-03 frame length (swapped) */
	char physaddr[6];	/* 04-09 physical address */
	int  rec_frames;	/* 10-11 # frames received (swapped) */
	int  fifo_frames;	/* 12-13 # fifo frames received (swapped) */
	int  xmit_frames;	/* 14-15 # frames transmitted (swapped) */
	int  excess_coll;	/* 16-17 # of excess collisions (swapped) */
	int  coll_frag;		/* 18-19 # of collision fragments (swapped) */
	int  lost_frames;	/* 20-21 # of times >=1 frame lost (swapped) */
	int  mcast_accepted;	/* 22-23 # of multicast frms accptd (swapped)*/
	int  mcast_rejected;	/* 24-25 # of multicast frms rjctd (swapped) */
	int  crc_errors;	/* 26-27 # of crc frames errors (swapped) */
	int  align_errors;	/* 28-29 # of alignment errors (swapped) */
	int  collsions;		/* 30-31 # of collisions (swapped) */
	int  out_of_wind;	/* 32-33 # of out-of-window collisions (swap)*/
	char null1[16];		/* 34-49 Reserved for future use */
	char module_id[7];	/* 50-56 Module id */
	char null2;		/* 57 null */
	char firmware_id[7];	/* 58-64 firmware id */
	char null3;		/* 65 null */
};

	/* ethernet version of device independent communication block */

struct etcom {
	struct etcom *et_next;			
	char et_body[HEAD+DATOFF+CBHDR+RBSIZE+CRCSIZ];
};
