
	/*  fsdb - file system debugger    */

#include	<sys/param.h>
#include	<sys/inode.h>
#include	<sys/ino.h>
#include	<stdio.h>
#include <setjmp.h>
#ifdef STANDALONE
#define ASSEM "Stand-Alone"
#else
#define ASSEM "SYS3"
#endif
#define	NBUF     3
#define	MODE     0
#define	LINK     2
#define	UID      4
#define	GID      6
#define	SZ       (BSIZE/64)
#define	ADRMSK	(BSIZE -1)
#define	A0       12
#define	MIN      12
#define	MAJ      13
#define	AT      52
#define	MT      56
#define	CT      60
#define	NUMB    10
#define	INODE   11
#define	BLOCK   12
#define	DIR     13

#define	HIBIT   0100000

#ifdef vax
#define SLOC (SUPERB*BSIZE + 4)
#else
#define SLOC (SUPERB*BSIZE + 2)
#endif

struct buf {
	struct	buf  *fwd;
	struct	buf  *back;
	char	*blkaddr;
	short	valid;
	long	blkno;
} buf[NBUF], bhdr;
char buffers[NBUF][BSIZE];

struct dir {
	short	inumb;
	char nm[DIRSIZ];
} *dirp;

struct dinode *ip;

long	addr, value, temp, oldaddr, oldvalue, erraddr;
long	cur_ino = (SUPERB+1)*BSIZE;;
short	objsz =  2;
short	fd, c_count, i, j, k, oldobjsz, error, type;
short	count, pid, rpid, retcode, mode, char_flag, prt_flag;
unsigned	isize;
long	fsize;
char	*p,
	*getblk();
short	override = 1;
jmp_buf env;

/*
 * main - continuously looping input scanner.  Lines are scanned
 * a character at a time and are processed as soon as
 * sufficient information has been collected. When an error
 * is detected, the remainder of the line is flushed.
 */

main(argc,argv)
	short	argc;
	char	**argv;
{

	register	char  *cptr;
	register	char c;
	register	short  *iptr;
#ifdef STANDALONE
#ifdef STOCKIII
	char devnam[9];
#else
	char devnam[20];
#endif
	int maj,min;
	long blk;
#endif
	extern	long get();
	extern	long bmap();
	extern	long getnumb();
	extern	err();
	register struct buf *bp;
	unsigned block;
	short offset;

/*  printf("version %I% - %G% %lu%\n\n");*/
#ifdef STOCKIII
	setbuf(stdin,NULL);

	if(argc!=2 && argc!=3) {
		printf("usage: fsdb /dev/dk??\n");
		exit(1);
	}
#endif

#ifdef vax
	printf("\nVAX %s asembly\n",ASSEM);
#else
#ifdef pdp11
	printf("\nPLEXUS %s assembly\n",ASSEM);
#endif
#endif
	if((fd = open(argv[1],2)) < 0) {

#ifdef STANDALONE
#ifndef STOCKIII
		if (argv[0][0] == '\0')
			argc = getargv ("fsdb", &argv, 0);
		cptr = argv[1];
		for(maj = 0; maj <sizeof(devnam); devnam[maj++] = *cptr++);
#else
		cptr = argv[1];
		for(maj = 0; maj <9; devnam[maj++] = *cptr++);
		if(devnam[8] == '\0'){
			maj = 0;
			min = devnam[7] - '0';
		}
		else {
			maj = devnam[7];
			min = devnam[8];
		}
		if(maj <0 | maj > 3 | min < 0 | min > 5){
			printf("invalid device name - %s\n",devnam);
			exit(1);
		}
		if(min == 0) blk = 0l;
		else blk = 1392l + ((long)(min-1) * 65626l);
printf("trying to make node %s %d %d %ld\n",devnam,maj,min,blk);
		if(MKNOD(devnam,maj,min,blk) < 0){
			printf("cannot create %s \n",devnam);
			exit(1);
		}
#endif
		if((fd = open(devnam,2)) < 0){
			printf("cannot open %s\n",argv[1]);
			exit(1);
		}
	}
#else
		printf("cannot open %s\n",argv[1]);
		exit(1);
	}
#endif

	bhdr.fwd = bhdr.back = &bhdr;
	for(i=0; i<NBUF; i++) {
		bp = &buf[i];
		bp->blkaddr = buffers[i];
		bp->valid = 0;
		insert(bp);
	}

	reload();
	if(argc == 3) printf("error checking off\n");
	else override = 0;

#ifndef STANDALONE
	signal(2,err);
	setjmp(env);
#endif

	for(;;) {
		if(error) {
			if(c != '\n') while (getc(stdin) != '\n');
			c_count = 0;
			prt_flag = 0;
			char_flag = 0;
			error = 0;
			addr = erraddr;
			printf("?\n");
			/* type = -1; allows "d31 + +" to work */
		}
		c_count++;

		switch(c = getc(stdin)) {

		case '\n': /* command end */
			erraddr = addr;
			if(c_count == 1) addr = addr + objsz;
			c_count = 0;
			if(prt_flag) {
				prt_flag = 0;
				continue;
			}
			temp = get(objsz);
			/* if an error has been flagged, it is probably
			 * due to allignment.  This will have set objsz
			 * to 1 hence the next get should work.
			 */
			if(error) temp = get(objsz);
			switch(objsz) {
			case 1: cptr = ".B"; break;
			case 2: cptr = ""; break;
			case 3: cptr = ".A"; break;
			case 4: cptr = ".D"; break;
			case DIRSIZ:
			case 16:
				if(bcomp(addr,erraddr)) continue;
				fprnt('d', 1);
				prt_flag = 0;
				continue;
			case 64: fprnt('i',1);
				cur_ino = addr;
				prt_flag = 0;
				continue;
			}
			printf("%6.6lo%s: %6.6lo (%ld)\n",addr,cptr,temp,temp);
			continue;

		default: /* catch absolute addresses, b and i#'s */
			if(c<='9' && c>='0') {
				ungetc(c,stdin);
				addr = getnumb();
				objsz = 2;
				value = addr;
				type = NUMB;
				continue;
			}
			if(feof(stdin)) exit(0);
			error++;
			continue;

		case 'i': /* i# to inode conversion */
			if(c_count == 1) {
				addr = cur_ino;
				value = get(64);
				type = INODE;
				continue;
			}
			if(type==NUMB)value = addr;
			addr = ((value - 1) << 6) + ((SUPERB+1)*BSIZE);
			if(icheck(addr)) continue;
			cur_ino = addr;
			value = get(64);
			type = INODE;
			continue;

		case 'b': /* block conversion */
			if(type == NUMB)value = addr;
			addr = value << BSHIFT;
			value = get(2);
			type = BLOCK;
			continue;

		case 'd': /* directory offsets */
			value = getnumb();
			if(error||(value > 31)) {
				error++;
				continue;
			}
			if(value != 0) if(dircheck()) continue;
			addr = (addr & ~ADRMSK) + (value << 4);
			value = get(16); /* i-number */
			type = DIR;
			continue;

		case '\t':
		case ' ':
		case '.': continue;

		case '+': /* address addition */
			c = getc(stdin);
			ungetc(c,stdin);
			if(c > '9' || c < '0') temp = objsz;
			else temp = getnumb() * objsz;
			if(error) continue;
			if(objsz == DIRSIZ || objsz == 16)
				if(bcomp(addr,addr+temp)) {
					c = '+';
					continue;
				}
			addr = addr + temp;
			value = get(objsz);
			continue;

		case '-': /* address subtraction */
			c = getc(stdin);
			ungetc(c,stdin);
			if(c > '9' || c < '0') temp = objsz;
			else temp = getnumb() * objsz;
			if(error) continue;
			if(objsz == DIRSIZ || objsz == 16)
				if(bcomp(addr,addr-temp)) {
					c = '-';
					continue;
				}
			addr = addr - temp;
			value = get(objsz);
			continue;

		case '*': temp = getnumb();
			if(error) continue;
			addr = addr * temp;
			value = get(objsz);
			continue;

		case '/': temp = getnumb();
			if(error) continue;
			addr = addr / temp;
			value = get(objsz);
			continue;

		case 'q': /* quit */
			if(c_count != 1 || (c = getc(stdin)) != '\n') {
				error++;
				continue;
			}
			exit(0);

		case '>': /* save current address */
			oldaddr = addr;
			oldvalue = value;
			oldobjsz = objsz;
			continue;

		case '<': /* restore saved address */
			addr = oldaddr;
			value = oldvalue;
			objsz = oldobjsz;
			continue;

		case 'a': /* access time */
			if((c = getc(stdin)) == 't') {
				addr = cur_ino + AT;
				type = AT;
				value = get(4);
				continue;
			}

			/* data block addresses */
			ungetc(c,stdin);
			value = getnumb();
			if(error||(value > 12)) {
				error++;
				continue;
			}
			addr = cur_ino + A0 + (value * 3);
			value = get(3);
			type = A0;
			continue;

		case 'm': /* mt, md, maj, min */
			addr = cur_ino;
			mode = get(2);
			if(error) continue;
			switch(c = getc(stdin)) {
			case 't': /* modification time */
				addr = addr + MT;
				type = MT;
				value = get(4);
				continue;
			case 'd': /* mode */
				addr = addr + MODE;
				type = MODE;
				value = get(2);
				continue;
			case 'a': /* major device number */
				if((c = getc(stdin)) != 'j') {
					error++;
					continue;
				}
				if(devcheck(mode)) continue;
				addr = addr + MAJ;
				value = get(1);
				type = MAJ;
				continue;
			case 'i': /* minor device number */
				if((c = getc(stdin)) != 'n') {
					error++;
					continue;
				}
				if(devcheck(mode)) continue;
				addr = addr + MIN;
				value = get(1);
				type = MIN;
				continue;
			}
			error++;
			continue;

		case 's': /* file size */
			if((c = getc(stdin)) != 'z') {
				error++;
				continue;
			}
			addr = cur_ino + SZ;
			value = get(4);
			type = SZ;
			continue;

		case 'l': /* link count */
			if((c = getc(stdin)) != 'n') {
				error++;
				continue;
			}
			addr = cur_ino + LINK;
			value = get(2);
			type = LINK;
			continue;

		case 'g': /* group id */
			if((c=getc(stdin))!= 'i' || (c=getc(stdin)) != 'd') {
				error++;
				continue;
			}
			addr = cur_ino + GID;
			value = get(2);
			type = GID;
			continue;

		case 'u': /* user id */
			if((c=getc(stdin))!= 'i' || (c=getc(stdin)) != 'd') {
				error++;
				continue;
			}
			addr = cur_ino + UID;
			value = get(1);
			type = UID;
			continue;

		case 'n': /* directory name */
			if((c = getc(stdin)) != 'm') {
				error++;
				continue;
			}
			if(dircheck()) continue;
			type = DIRSIZ;
			objsz = DIRSIZ;
			addr = (addr & ~017) + 2;
			continue;

		case '=': /* assignment operation	*/
			switch(c = getc(stdin)) {
			case '"': /* character string */
				puta();
				continue;
			case '+': /* =+ operator */
				temp = getnumb();
				value = get(objsz);
				if(!error) put(value+temp,objsz);
				continue;
			case '-': /* =- operator */
				temp = getnumb();
				value = get(objsz);
				if(!error) put(value-temp,objsz);
				continue;
			default: /* nm and regular assignment */
				ungetc(c,stdin);
				if((type == DIRSIZ) && (c > '9' || c < '0')) {
					puta();
					continue;
				}
				value = getnumb();
				if(!error) put(value,objsz);
				continue;
			}

		case '!': /* shell command */
#ifdef STANDALONE
			printf("Shell not available stand-alone");
			continue;
#else
			if(c_count != 1) {
				error++;
				continue;
			}
			if((pid = fork()) == 0) {
				execl("/bin/sh", "sh", "-t", 0);
				error++;
				continue;
			}
			while((rpid = wait(&retcode)) != pid && rpid != -1);
			printf("!\n");
			c_count = 0;
			continue;
#endif

		case 'F': /* buffer status */
			for(bp=bhdr.fwd; bp!= &bhdr; bp=bp->fwd)
				printf("%6lu %d\n",bp->blkno,bp->valid);
			continue;

		case 'f': /* file print facility */
			if((c=getc(stdin)) >= '0' && c <= '9') {
				ungetc(c,stdin);
				temp = getnumb();
				if (error) continue;
				c = getc(stdin);
			}
			else temp = 0;
			count = 0;
			addr = cur_ino;
			mode = get(2);
			if(!override) {
				if((mode & IFMT)==0)
					printf("warning: inode not allocated\n");
			}

			if(mode & IFCHR) {
				printf("special device\n");
				error++;
				continue;
			}

			if((addr = (bmap(temp)) << BSHIFT) == 0)
				continue;
			fprnt(c,0);
			continue;

		case 'O': /* override flip flop */
			if(override = !override)
				printf("error checking off\n");
			else {
				printf("error checking on\n");
				reload();
			}
			prt_flag++;
			continue;

		case 'B': /* byte offsets */
			objsz = 1;
			continue;

		case 'W': /* word offsets */
			objsz = 2;
			addr = addr & ~01;
			continue;

		case 'D': /* double word offsets */
			objsz = 4;
			addr = addr & ~01;
			continue;
		case 'A': abort();

		case ',': /* general print facilities */
		case 'p':
			if(( c = getc(stdin)) >= '0' && c <= '9') {
				ungetc(c,stdin);
				count = getnumb();
				if(error) continue;
				if((count < 0) || (count > BSIZE))
					count = 1;
				c = getc(stdin);
			}
			else if(c == '*'){
				count = 0;	/* print to end of block */
				c = getc(stdin);
			}
			else count = 1;
			fprnt(c,count);
		}
	}
}

/*
 * getnumb - read a number from the input stream.  A leading
 * zero signifies octal interpretation. If the first character
 * is not numeric this is an error, otherwise continue
 * until the first non-numeric.
 */

long
getnumb()
{

	extern	short  error;
	long	number, base;
	register	char  c;

	if(((c = getc(stdin)) < '0')||(c > '9')) {
		error++;
		ungetc(c,stdin);
		return(-1);
	}
	if(c == '0') base = 8;
	else base = 10;
	number = c - '0';
	while(((c = getc(stdin)) >= '0' )&&( c <= '9')) {
		if((base == 8) && ((c =='8')||(c == '9'))) {
			error++;
			return(-1);
		}
		number = number * base + c - '0';
	}
	ungetc(c,stdin);
	return(number);
}

/*
 * get - read a byte, word or double word from the file system.
 * The entire block containing the desired item is read
 * and the appropriate data is extracted and returned. 
 * Inode and directory size requests result in word
 * fetches. Directory names (objsz == DIRSIZ) result in byte
 * fetches.
 */

long
get(lngth)
	short	lngth;
{

	long	vtemp;
	char *bptr;
	char *vptr;
	short offset;

	objsz = lngth;
	if(allign(objsz)) return(-1);
	if((bptr = getblk(addr)) == 0) return(-1);
	vtemp = 0;
	offset = addr & ADRMSK;
	bptr = bptr + offset;
	if(offset + lngth > BSIZE) {
		error++;
printf ("get(lngth) - %d too long\n",offset + lngth);
		return(-1);
	}
	switch(objsz) {
	case 4:	vtemp = *(long *)bptr;
		return(vtemp);
	case 64:
	case 16:
	case 2: loword(vtemp) = *(short *)bptr;
		return(vtemp);
	case DIRSIZ:
	case 1: lobyte(loword(vtemp)) = *bptr;
		return(vtemp);
	case 3:
		l3tol(&vtemp,bptr,1);
		return(vtemp);
	}
	error++;
printf ("get(%d) - invalid length\n",lngth);
	return(0);
}

/*
 * icheck - check if the current address is within the I-list.
 * The I-list extends for isize blocks beginning at the 
 * super block + 1, i.e., from block 2 to block isize.
 */

icheck(address)
	long	address;
{
	unsigned blk;

	if(override) return(0);
	blk = address >> BSHIFT;
	if((blk >= 2) && (blk < isize))
		return(0);
	printf("inode out of range\n");
	error++;
	return(1);
}

/*
 * putf - print a byte as an ascii character if possible.
 * The exceptions are tabs, newlines, backslashes
 * and nulls which are printed as the standard c
 * language escapes. Characters which are not
 * recognized are printed as \?.
 */

putf(c)
	register	char  c;
{

	if(c<=037 || c>=0177 || c=='\\') {
		putc('\\',stdout);
		switch(c) {
		case '\\': putc('\\',stdout);
			break;
		case '\t': putc('t',stdout);
			break;
		case '\n': putc('n',stdout);
			break;
		case '\0': putc('0',stdout);
			break;
		default: putc('?',stdout);
		}
	}
	else {
		putc(' ',stdout);
		putc(c,stdout);
	}
	putc(' ',stdout);
}

/*
 * put - write an item into the buffer for the current address
 * block.  The value is checked to make sure that it will
 * fit in the size given without truncation.  If successful,
 * the entire block is written back to the file system.
 */

put(item,lngth)
	long item;
	short lngth;
{

	register char *bptr, *sbptr, *vptr;
	short offset;
	long	s_err,nbytes;

	objsz = lngth;
	if(allign(objsz)) return(-1);
	if((sbptr = getblk(addr)) == 0) return;
	offset = addr & ADRMSK;
	if(offset + lngth > BSIZE) {
		error++;
		printf("block overflow\n");
		return;
	}
	bptr = sbptr + offset;
	switch(objsz) {
	case 3: vptr = ((char *)&item);
		if(vptr[1] != 0) break;
		ltol3(bptr,vptr,1);
		goto rite;
	case 4: *bptr = item;
		goto rite;
	case 64:
	case 16:
	case 2: if(item & ~0177777L) break;
		*(short *)bptr = item;
		goto rite;
	case DIRSIZ:
	case 1: if(item & ~0377) break;
		*bptr = lobyte(loword(item));
rite:	if((s_err = lseek(fd,addr & ~(long)ADRMSK,0)) == -1) {
		error++;
		printf("seek error : %lo\n",addr);
		return(0);
	}
	if((nbytes = write(fd,sbptr,BSIZE)) != BSIZE) {
		error++;
		printf("write error : addr   = %lo\n",addr);
		printf("           : s_err  = %lo\n",s_err);
		printf("           : nbytes = %lo\n",nbytes);
		return(0);
	}
		return;
	default: error++;
		return;
	}
	printf("truncation error\n");
	error++;
}

/*
 * getblk - check if the desired block is in the file system.
 * Search the incore buffers to see if the block is already
 * available. If successful, unlink the buffer control block
 * from its position in the buffer list and re-insert it at
 * the head of the list.  If failure, use the last buffer
 * in the list for the desired block. Again, this control
 * block is placed at the head of the list. This process
 * will leave commonly requested blocks in the in-core buffers.
 * Finally, a pointer to the buffer is returned.
 */

char *getblk(address)
	long	address;
{

	register struct buf *bp;
	long	block;
	long	s_err,nbytes;

	block = addr >> BSHIFT;
	if(!override)
		if(block >= fsize) {
			printf("block out of range\n");
			error++;
			return(0);
		}
	for(bp=bhdr.fwd; bp!= &bhdr; bp=bp->fwd)
		if(bp->blkno==block && bp->valid) goto xit;
	bp = bhdr.back;
	bp->blkno = block;
	bp->valid = 0;

	if((s_err = lseek(fd,addr & ~(long)ADRMSK,0)) == -1) {
		error++;
		printf("seek error : %lo\n",addr);
		return(0);
	}
	if((nbytes = read(fd,bp->blkaddr,BSIZE)) != BSIZE) {
		error++;
		printf("read error : addr   = %lo\n",addr);
		printf("           : s_err  = %lo\n",s_err);
		printf("           : nbytes = %lo\n",nbytes);
		return(0);
	}
	bp->valid++;
xit:	bp->back->fwd = bp->fwd;
	bp->fwd->back = bp->back;
	insert(bp);
	return(bp->blkaddr);
}

/*
 * insert - place the designated buffer control block
 * at the head of the linked list of buffers.
 */

insert(bp)
	register struct buf *bp;
{

	bp->back = &bhdr;
	bp->fwd = bhdr.fwd;
	bhdr.fwd->back = bp;
	bhdr.fwd = bp;
}

/*
 * allign - before a get or put operation check the 
 * current address for a boundary corresponding to the 
 * size of the object.
 */

allign(ment)
	short ment;
{

	switch(ment) {
	case 4: if(addr & 01L) break;
		return(0);
	case DIRSIZ: if((addr & 017) != 02) break;
		return(0);
	case 16:
	case 64:
	case 2: if(addr & 01L) break;
	case 3:
	case 1: return(0);
	}
	error++;
	objsz = 1;
	printf("allignment\n");
	return(1);
}

/*
 * err - called on interrupts.  Set the current address
 * back to the last address stored in erraddr. Reset all
 * appropriate flags.  If the prt_flag is set, the interrupt
 * occured while transferring characters to a buffer. 
 * These are "erased" by invalidating the buffer, causing
 * the entire block to be re-read upon the next reference.
 * A reset call is made to return to the main loop;
 */

err()
{
#ifndef STANDALONE
	signal(2,err);
	addr = erraddr;
	error = 0;
	c_count = 0;
	if(char_flag) {
		bhdr.fwd->valid = 0;
		char_flag = 0;
	}
	prt_flag = 0;
	printf("\n?\n");
	fseek(stdin, 0L, 2);
	longjmp(env,0);
#endif
}

/*
 * devcheck - check that the given mode represents a 
 * special device. The IFCHR bit is on for both
 * character and block devices.
 */

devcheck(md)
	register short md;
{
	if(override) return(0);
	if(md & IFCHR) return(0);
	printf("not char or block device\n");
	error++;
	return(1);
}

/*
 * nullblk - return error if address is zero.  This is done
 * to prevent block 0 from being used as an indirect block
 * for a large file or as a data block for a small file.
 */

nullblk(bn)
	long	bn;
{
	if(bn != 0) return(0);
	printf("non existent block\n");
	error++;
	return(1);
}

/*
 * dircheck - check if the current address can be in a directory.
 * This means it is not in the I-list, block 0 or the super block.
 */

dircheck()
{
	unsigned block;

	if(override) return (0);
	if((block = (addr >> BSHIFT)) >= isize) return(0);
	error++;
	printf("block in I-list\n");
	return(1);
}

/*
 * puta - put ascii characters into a buffer.  The string
 * terminates with a quote or newline.  The leading quote,
 * which is optional for directory names, was stripped off
 * by the assignment case in the main loop.  If the type
 * indicates a directory name, the entry is null padded to
 * DIRSIZ bytes.  If more than 14 characters have been given
 * with this type or, in any case, if a block overflow
 * occurs, the current block is made invalid. See the 
 * description for err.
 */

puta()
{
	register char *bptr, c;
	register offset;
	char *sbptr;
	unsigned block;
	long	s_err,nbytes;

	if((sbptr = getblk(addr)) == 0) return;
	char_flag++;
	offset = addr & ADRMSK;
	bptr = sbptr + offset;

	while((c = getc(stdin)) != '"') {
		if(offset++ == BSIZE) {
			bhdr.fwd->valid = 0;
			error++;
			char_flag = 0;
			printf("block overflow\n");
			return;
		}
		if(c == '\n') {
			ungetc(c,stdin);
			break;
		}
		if(c == '\\') {
			switch(c = getc(stdin)) {
			case 't': *bptr++ = '\t'; break;
			case 'n': *bptr++ = '\n'; break;
			case '0': *bptr++ = '\0'; break;
			default: *bptr++ = c; break;
			}
		}
		else *bptr++ = c;
	}
	char_flag = 0;
	if(type == DIRSIZ) {
		c = offset - (addr & ADRMSK);
		if(c > DIRSIZ) {
			bhdr.fwd->valid = 0;
			error++;
			char_flag = 0;
			printf("name too long\n");
			return;
		}
		while(c++ < DIRSIZ) *bptr++ = '\0';
	}
	if((s_err = lseek(fd,addr & ~(long)ADRMSK,0)) == -1) {
		error++;
		printf("seek error : %lo\n",addr);
		return(0);
	}
	if((nbytes = write(fd,sbptr,BSIZE)) != BSIZE) {
		error++;
		printf("write error : addr   = %lo\n",addr);
		printf("           : s_err  = %lo\n",s_err);
		printf("           : nbytes = %lo\n",nbytes);
		return(0);
	}
}

/*
 * fprnt - print data as characters, octal or decimal words, octal
 * bytes, directories or inodes	A total of "count" entries
 * are printed. A zero count will print all entries to the 
 * end of the current block.  If the printing would cross a
 * block boundary, the attempt is aborted and an error returned.
 * This is done since logically sequential blocks are generally
 * not physically sequential.  The error address is set
 * after each entry is printed.  Upon completion, the current
 * address is set to that of the last entry printed.
 */

fprnt(style,count)
	register char style;
	register short	count;
{
	short offset;
	unsigned block;
	char *cptr;
	short *iptr;
	short *tptr;
	long tmp;

	prt_flag++;
	block = addr >> BSHIFT;
	offset = addr & ADRMSK;
	if((cptr = getblk(addr)) == 0){
		ip = 0;
		iptr = 0;
		dirp = 0;
		return;
	}
	ip = ((struct dinode *)cptr);
	erraddr = addr;

	switch (style) {

	case 'c': /* print as characters */
	case 'b': /* or octal bytes */
		if(count == 0) count = BSIZE - offset;
		if(offset + count > BSIZE) break;
		objsz = 1;
		cptr = cptr + offset;
		for(i=0; count--; i++) {
			if(i % 16 == 0)
				printf("\n%6.6lo: ",addr);
			if(style == 'c') putf(*cptr++);
			else printf("%4.4o",*cptr++ & 0377);
			erraddr = addr;
			addr++;
		}
		addr--;
		putc('\n',stdout);
		return;

	case 'd': /* print as directories */
		if(dircheck()) return;
		addr = addr & ~017;
		offset = offset >> 4;
		if(count == 0) count = 32 - offset;
		if(count + offset > 32) break;
		type = DIR; 
		objsz = 16;
		for(dirp = ((struct dir *)cptr) + offset; count--; dirp++) {
			printf("d%d: %4d  ",offset++,dirp->inumb);
			cptr = dirp->nm;
			for(j=0; j<DIRSIZ; j++) {
				if(*cptr == '\0') break;
				putf(*cptr++);
			}
			putc('\n',stdout);
			erraddr = addr;
			addr = addr + 16;
		}
		addr = erraddr;
		return;

	case 'o': /* print as octal words */
	case 'e': /* print as decimal words */
		addr = addr & ~01;
		offset = offset >> 1;
		iptr = ((short *)cptr) + offset;
		if(count == 0) count = (BSIZE/sizeof(int)) - offset;
		if(offset + count > (BSIZE/sizeof(int))) break;
		objsz = 2;
		for(i=0; count--; i++) {
			if(i % 8 == 0) {

				/*  this code deletes lines of zeros  */
				tptr = iptr;
				k = count -1;	/* always print last line */
				for(j = i; k--; j++)
				if(*tptr++ != 0) break;
				if(j > (i + 7)) {
					j = (j - i) >> 3;
					while(j-- > 0){
						iptr = iptr + 8;
						count = count - 8;
						i = i + 8;
						addr = addr + 16; 
					}
					printf("\n*");
				}

				printf("\n%6.7lo:",addr);
			}
			if(style == 'o')printf("  %6.6o",*iptr++ & 0177777);
			else printf("  %6d",*iptr++);
			erraddr = addr;
			addr = addr + 2;
		}
		addr = erraddr;
		putc('\n',stdout);
		return;

	case 'i': /* print as inodes */
		if(icheck(addr)) return;
		addr = addr & ~077;
		offset = offset >> 6;
		if(count == 0) count = INOPB - offset;
		if(count + offset > INOPB) break;
		type = INODE;
		objsz = 64;
		ip = ip + offset;
		temp = (addr - ((SUPERB+1)*BSIZE)) / 64 + 1;
		for(i=0; count--; ip++) {
			printf("i#:%6ld  md: ",temp++);
			p = " ugtrwxrwxrwx";
			mode = ip->di_mode;
			switch(mode & IFMT) {
			case IFDIR: putc('d',stdout); break;
			case IFCHR: putc('c',stdout); break;
			case IFBLK: putc('b',stdout); break;
			case IFREG: putc('f',stdout); break;
			case IFIFO: putc('p',stdout); break;
			default: putc('-',stdout); break;
			}
			for(mode = mode << 4; *++p; mode = mode << 1) {
				if(mode & HIBIT) putc(*p,stdout);
				else putc('-',stdout);
			}
			printf("  ln:%5d  uid:%5d  gid:%5d",
				ip->di_nlink,ip->di_uid,ip->di_gid);
			printf("  sz: %8lu\n", ip->di_size);
			if(ip->di_mode & IFCHR) 
				printf("maj:%3.3o  min:%3.3o  ",
					ip->di_addr[1] & 0377, ip->di_addr[0] & 0377);
			else {
				for(i = 0; i < NADDR; i++) {
					l3tol(&tmp,&ip->di_addr[3*i],1);
					printf("a%d:%6lu  ",i,tmp);
					if(i == 6) putc('\n',stdout);
				}
				putc('\n',stdout);
			}
			printf("at: %s",ctime(&ip->di_atime));
			printf("mt: %s",ctime(&ip->di_mtime));
			printf("ct: %s",ctime(&ip->di_ctime));
			if(count) putc('\n',stdout);
			cur_ino = erraddr = addr;
			addr = addr + 64;
		}
		addr = erraddr;
		return;


	default: error++;
		printf("no such print option\n");
		return;
	}
	error++;
	printf("block overflow\n");
}

/*
 * reload - read new values for isize and fsize. These are
 * the basis for most of the error checking procedures.
 */

reload()
{
	long saddr;
	short	sv_objsz;

	saddr = addr;
	sv_objsz = objsz;
	addr = SUPERB*BSIZE;;
	isize = get(2);
	addr = SLOC;
	fsize = get(4);
	addr = saddr;
	objsz = sv_objsz;
	if(error) printf("cannot read superblock\n");
	else printf("fsize = %ld, isize = %u\n",fsize,(isize - 2) * INOPB);
}

/*
 * bcomp - compare the block numbers of two long addresses.
 * Used to check for block over/under flows when stepping through
 * a file system.
 */

bcomp(addr1,addr2)
	long addr1;
	long addr2;
{
	if(override) return(0);
	if((addr1 & ~(long)ADRMSK) == (addr2 & ~(long)ADRMSK)) return(0);
	error++;
	printf("block overflow\n");
	return(1);
}

/*
 * bmap - maps the logical block number of a file into
 * the corresponding physical block on the file
 * system.  Algorithm is that of the version 7 bmap routine.
 */

long
bmap(bn)
	long	bn;
{

	long	nb;
	int	j, sh, i;

	addr = cur_ino;
	if(bn < NADDR - 3) {
		addr += A0 + bn * 3;
		return(nullblk(nb=get(3)) ? 0L : nb);
	}

	sh = 0;
	nb = 1;
	bn -= NADDR - 3;
	for(j=3; j>0; j--) {
		sh += NSHIFT;
		nb = nb << NSHIFT;
		if(bn < nb)
			break;
		bn -= nb;
	}
	if(j==0) {
		error++;
		printf("file too big\n");
		return(0L);
	}
	addr += A0 + (NADDR - j) * 3;
	if(nullblk(nb=get(3)))
		return(0L);
	for(; j<=3; j++) {
		sh -= NSHIFT;
		addr = (nb << BSHIFT) + 4 * ((bn >> sh) & NMASK);
		if(nullblk(nb=get(4)))
			return(0L);
	}
	return(nb);
}
