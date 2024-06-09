
char buf[50];

typedef char *menulist;

menulist moreinfo[] = {
"",
"",
"	MORE INFORMATION ABOUT THE HELP PROGRAM",
"",
"",
"	WELCOME TO PLEXUS SYS3 1.1 !!!",
"",
"",
"	This tape contains the programs needed to bring up software",
"	on a new system or restore software on a system that has been",
"	corrupted in some way.",
"",
"	To find out about the different programs on this tape, type",
"	the number in the left column that corresponds to the program",
"	you want, followed by a carriage return.",
"",
"	Type 1 followed by a carriage return to display the menu.",
"",
"	Type 2 followed by a carriage return to display the tape",
"	contents.",
0 };

menulist firstmenu[] = {
"",
"	PLEXUS SOFTWARE RELEASE -- SYS3 REV 1.1",
"",
"	(-1)	Return to boot program",
"	(0)	More information",
"	(1)	This menu",
"	(2)	Display tape contents",
"	(3)	Recreating system software on disk",
"	(4)	The standalone programs",
"	(5)	Formatting the disk (dformat)",
"	(6)	Making a file system (mkfs)",
"	(7)	Restoring from a dump tape (restor)",
"	(8)	Configuring block 0 of the disk (dconfig)",
"",
"",
"",
"",
"",
"",
"",
0 };

menulist distmenu[] = {
"",
"	DISPLAY TAPE CONTENTS",
"",
"	File 0:	The help program (help)",
"	File 1:	The standalone bootstrap (secondary boot)",
"	File 2:	A copy of Plexus sys3 (sys3)",
"	File 3:	The disk formatting program (dformat)",
"	File 4:	Construct a file system (mkfs)",
"	File 5:	File system restore (restor)",
"	File 6:	File system consistancy check (icheck)",
"	File 7:	A device to device copy program (dd)",
"	File 8:	A fast backup from disk to tape (fbackup)",
"	File 9:	A file dump program (od)",
"	File 10:	The disk configuration program (dconfig)",
"	File 11:	Reserved for future use",
"	File 12:	A file system debugger (fsdb)",
"	File 13:	A program to summarize disk usage (du)",
"	File 14:	List contents of directories (ls)",
"	File 15:	Concatenate and print files (cat)",
"",
"\1",
"",
"	Files 16-19:	Empty",
"	File 20:	A dump of the Plexus SYS3 file system",
"	File 21:	Has files changed between Release 1.0 and 1.1 of",
"			 Plexus SYS3 in cpio format",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"Hit 'return' for more help.",
0 };

menulist recreatemenu[] = {
"",
"		RECREATING SYSTEM SOFTWARE ON DISK",
"",
"	To bring up system software on a new disk or to reformat a",
"	a disk for any reason:",
"",
"	    (1) Turn on system power or press reset button.",
"	    (2) Wait for 'PLEXUS SELFTEST COMPLETE' message.",
"	    (3) Wait for 'Boot' message and ':' prompt.",
"	    (4) Load a standard Plexus SYS3 release tape.",
"	    (5) Run the following sequence of programs:",
"",
"		dformat -- standalone disk format and spare tracks",
"		(Note: the dformat program should be run only in the",
"		event of catastrophic failure, since it completely",
"		reformats the disk.)",
"",
"		mkfs -- standalone make file system",
"",
"		restor -- standalone restor file system from dumptape",
"",
"\1",
"",
"	After completing the sequence of standalone programs, follow",
"	normal startup procedures (answer <return> to the boot prompt).",
"	At this point, sys3 UNIX should come up in a single-user mode",
"	(indicated by the '#' prompt).  Then type 'init 2<return>' to",
"	bring the system up in multi-user mode.  This will normally",
"	print a login message at each terminal.",
"",
"",
"	Applying Software Updates",
"",
"	Refer to the installation information which accompanies each",
"	software product",
"",
"",
"",
"",
"",
"",
"Hit 'return' for more help.",
0 };

menulist standalonemenu[] = {
"",
"			STANDALONE PROGRAMS",
"",
"	Certain standalone programs must be run before booting UNIX.",
"	They perform a number of tasks necessary to allow UNIX to run",
"	on a new system.  Catastrophic system errors may also require",
"	that these programs be run.",
"",
"	Standalone programs accept device names and file names as arguments.",
"	Devices may be specified by a full pathname starting with a slash",
"	(e.g., '/dev/dk1' ) or by a device specifier followed by a unit",
"	number and an offset (e.g., 'is(0,0)').  A device specifier may",
"	be a disk device such as 'dk', 'is', or 'pd' in which case the",
"	offset is a 512 byte sector offset onto the disk.  For example,",
"	pd(0,40000) refers to the PD disk, unit 0, sector 40000.  A device",
"	specifier may also be a tape device such as 'mt', 'rm' or 'pt'",
"	in which case the offset is a file offset on the tape.  For",
"	example, mt(0,3) refers to file 3 on tape unit 0.  The first",
"	file on tape is file 0.",
"",
"\1",
"",
"	File names begin with an optional device specifier followed by",
"	a pathname.  Pathnames for files on disk must start with a slash,",
"	and refer to files on /dev/dk1.  If the device specifier is omitted,",
"	the default disk or tape specifier is assumed.",
"",
"	The standalone programs use the same disk and tape device names",
"	used in Sys3.  For example,",
"",
"		/dev/dk0 through /dev/dk15 refers to the logical",
"			volumes of the first default disk.",
"",
"		/dev/is0 through /dev/is15 refers to the logical",
"			volumes of the first iSBC disk.",
"",
"		/dev/pd0 through /dev/pd15 refers to the logical",
"			volumes of the first IMSC disk.",
"",
"		/dev/mt0 refers to the default tape drive.",
"",
"		/dev/nmt0 refers to the default tape drive, no rewind.",
"\1",
"",
"		/dev/rm0 refers to the 9-track tape drive.",
"",
"		/dev/nrm0 refers to the 9-track tape drive, no rewind.",
"",
"		/dev/pt0 refers to the tape cartridge.",
"",
"		/dev/npt0 refers to the tape cartridge, no rewind.",
"",
"	Standalone programs can all be executed from the SYS3 release tape",
"	and in some cases can also be executed from the disk.  To execute",
"	a standalone program from the release tape, only the program name",
"	must be typed in response to the boot prompt.  Note: if the system",
"	has a cartridge and a 9-track tape, the standalone program is",
"	executed from the cartridge.  For example,", 
"",
"	    PLEXUS PRIMARY BOOT REV 1.0",
"	    : mkfs",
"",
"	To execute a standalone program from disk requires the user to",
"	specify a complete pathname for the program.  For example:",
"\1",
"",
"	    PLEXUS PRIMARY BOOT REV 1.0",
"	    : /stand/mkfs",
"",
"	In this example, the program mkfs would be read in from the /stand",
"	directory on /dev/dk1 and executed.",
"",
"	To access mkfs from a file system that starts at sector 40000 of",
"	the second disk unit, the following response would work:",
"",
"	    : dk(1,40000)/stand/mkfs",
"",
"	Upon completion, standalone programs print 'Exit' followed by",
"	a value.  The value '0' indicates that the program completed",
"	normally.",
"",
"	Generally, a standalone program prompts once with its name for",
"	any parameters it needs. The parameters are the same as described",
"	in Volume 1 of the Plexus UNIX Programmer's Manual.",
"",
"",
"\1",
"",
"	The following gives some detail of each standalone programs:",
"",
"	    help -- Gives information about the release tape and use of",
"	    standalone programs.",
"",
"	    boot (secondary boot) -- Prompts for pathname, loads program",
"	    into memory and executes it.",
"",
"	    sys3 -- A binary copy of the SYS3 UNIX operating system kernel.",
"",
"	    dformat (disk format) -- Formats a disk drive, prompts for",
"	    format information.",
"",
"	    mkfs (make file system) -- Creates a bare file system.",
"	    Parameters are size and location.  See MKFS(1).",
"",
"	    restor (restore file system from dump tape) -- Parameters are",
"	    tape name, file system, and tape offset.  See RESTOR(1).",
"",
"",
"\1",
"",
"	    icheck (file system consistency check) -- Parameters are file",
"	    system name.  This is an alias for the /etc/fsck program.",
"	    See FSCK(1).",
"",
"	    dd (device-to-device copy program) -- Convert and copy a file",
"	    See DD(1).",
"",
"	    fbackup (fast backup) -- Does a quick image copy from disk",
"	    to tape or from tape to disk.  Prompts for parameters.",
"",
"	    od (octal dump) -- Dumps a file to the screen in octal or",
"	    hex format.  See OD(1).",
"",
"	    dconfig (configure block 0 of the disk) -- Prompts for disk",
"	    size and logical layout information.",
"",
"	    fsdb (file system debugger) -- Examine and patch file systems.",
"	    Parameter is a disk device containing file system.  See FSDB(1).",
"",
"",
"\1",
"",
"	    du (summarize disk usage) -- Prints the blocks used by the",
"	    files given as parameters.  See DU(1).",
"",
"	    ls (list contents of directories) -- Prints the name of the",
"	    files and their attributes given as parameters.  See LS(1).",
"",
"	    cat (concatenate and print files) -- Prints the contents of",
"	    the files given as parameters.  See CAT(1).",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"Hit 'return' for more help.",
0 };

menulist formatmenu[] = {
"",
"",
"			FORMATTING THE DISK",
"",
"",
"	dformat -- This program is used for formatting the disk,",
"		   sparing tracks, and initializing the disk.",
"",
"	(NOTE: Before initiating this procedure, have available a list",
"	of the bad sectors on the system disk drive.  Plexus supplies",
"	this list with each system shipped.  If this list is not readily",
"	available, the information can be obtained from the manufacturer's",
"	sticker affixed to the disk drive top.)",
"",
"	To initiate disk formatting, first ensure that a Plexus SYS3 release",
"	tape is properly installed in the tape drive.  The console will",
"	prompt as follows (typical response for the PLEXUS P/40 follows",
"	colon on each line):",
"",
"",
"",
"\1",
"",
"	    PLEXUS PRIMARY BOOT REV 1.0",
"	    : dformat",
"",
"	    Standalone dformat --",
"	    Format, spare, or initialize the disk? [fsi]: f",
"	    Unit? [0-3]: 0",
"	    Total number of cylinders? [3-1024]: 823",
"	    Number of heads on removable? [0-24]: 0",
"	    Number of heads on fixed? [0-24]: 5 {or 10}*",
"	    Data bytes per sector? [32-1024]: 512",
"	    Sectors per track [18-108]: 35",
"	    Number of alternate cylinders? [0-50]: 20",
"	    Interleave factor? [1-10]: 4",
"	    User pattern? [8 hex digits]: a5a5a5a5",
"	    Defective tracks on fixed?(cyl,head) [0,822,0-x]:",
"",
"	At this point, enter each defective track by typing the cylinder",
"	number followed by a comma and then the head number followed by a",
"	<return>.  When all defective tracks have been entered using this",
"	format, type <return> again, to continue formatting:",
"\1",
"",
"	    File system blocksize? [512-8192]: 1024",
"	    Default boot name?: /sys3",
"",
"	This step cause a delay of 5-10 min, depending upon disk size.",
"	During this time, the system is formatting the disk according to",
"	the user specifications just provided.  Upon completion, the",
"	system console displays:",
"",
"	    Disk formatted and initialized successfully!",
"	    Exit 0",
"",
"	    PLEXUS PRIMARY BOOT REV 1.0",
"	    :",
"",
"	___________________",
"	    * 5 for 72-Mbyte disks; 10 for 145-Mbyte disks.",
"",
"",
"Hit 'return' for more help.",
0 };

menulist mkfsmenu[] = {
"",
"",
"			MAKING A FILE SYSTEM",
"",
"	mkfs -- This program creates a logical file system on disk",
"",
"	This standalone program constructs a file system according",
"	to user-specified parameters.  The program prompts with",
"	'$$ mkfs ' and expects the disk device and file system size.",
"	See MKFS(1).",
"",
"	For example, to make a file system that is 18000 blocks long",
"	(1024 byte blocks), on /dev/dk1, respond to the boot program",
"	as follows: (user response follows '$$ mkfs ' prompt)",
"",
"",
"",
"",
"",
"",
"",
"\1",
"",
"	    PLEXUS PRIMARY BOOT REV 1.0",
"	    : mkfs",
"",
"	    $$ mkfs /dev/dk1 18000",
"	    isize = 4496",
"	    m/n = 3 500",
"	    Exit 0",
"",
"	    PLEXUS PRIMARY BOOT REV 1.0",
"	    :",
"",
"	Note that the mkfs on the SYS3 release tape was used.  If the SYS3",
"	file system is intact, it would also be possible to use the",
"	standalone mkfs from the disk; this could be done by typing",
"	the following in response to the boot prompt:",
"",
"	    /stand/mkfs",
"",
"",
0 };

menulist restormenu[] = {
"",
"",
"		RESTORE A FILE SYSTEM FROM DUMP TAPE",
"",
"	restor -- This program restores a file system from a dump tape.",
"",
"	(NOTE: Before attempting to use the standalone restor program,",
"	the standalone mkfs program must be run.)",
"",
"	To restore a file system from dump tape, after making a file",
"	system, make sure the release tape is loaded and type 'restor' in",
"	response to the boot prompt.  Wait for the restor prompt and then",
"	remove the release tape and load the dump tape before responding.",
"	The parameters to the restor standalone program are the name of the",
"	dump tape and the name of the file system (on disk).",
"",
"",
"",
"",
"",
"",
"\1",
"",
"	For example, to restore a file system from the 21st file (file 20)",
"	on the default tape to /dev/dk1, respond as indicated",
"	(response follows colon).",
"",
"	    PLEXUS PRIMARY BOOT REV 1.0",
"	    : restor",
"",
"	After typing 'restor', wait for restor prompt, remove release tape",
"	and load dump tape before responding to the '$$ restor ' prompt.",
"",
"	    $$ restor rf /dev/mt0 /dev/dk1 +20",
"	    Spacing forward 20 files on tape",
"	    Last chance before scribbling on disk. <return>",
"	    End of Tape",
"	    Exit 0",
"",
"	    PLEXUS PRIMARY BOOT REV 1.0",
"	    :",
"",
0 };

menulist dconfigmenu[] = {
"",
"",
"			CONFIGURING BLOCK 0 OF THE DISK",
"",
"",
"	dconfig -- this program configures a disk with initialization",
"		   information and the logical file system layout for SYS3.",
"",
"	This program only needs to be executed when it is desired to change",
"	the logical layout of the SYS3 file systems on disk.  It prompts",
"	for all needed information, giving the current value in brackets[].",
"",
"	This program should not be used, except by someone familiar with",
"	the layout of the file systems on disk.  Otherwise, it is possible",
"	to destroy both files and file systems and cause SYS3 to be",
"	unbootable.",
"",
"",
"",
"Hit 'return' for more help.",
0 };

menulist *menus[] = {
	moreinfo,
	firstmenu,
	distmenu,
	recreatemenu,
	standalonemenu,
	formatmenu,
	mkfsmenu,
	restormenu,
	dconfigmenu,
	0
	};

main() {
	int choice;

	choice = 2;

	for(;;) {
		if (choice >= (sizeof menus / 2) -1)
			choice = 1;
		displaymenu(choice);
		gets(buf);
		choice = atol(buf);
		if ((choice == -1) || (buf[0] == 'q'))
			exit(0);
	}
}

displaymenu(choice)
int choice;
{
	int i, j;

	putchar('\n');
	for(j=0;j<80;j++)
		putchar('*');
	putchar('\n');
	i = 0;
	while(menus[choice][i] != 0) {
		printf("%s\n",menus[choice][i]);
		if (menus[choice][i++][0] == 1) {
			printf("Type 'return' for more information about this item ... ");
			if (getchar() != '\n')
				return;
			for (j=0; j<80; j++)
				putchar('+');
			putchar('\n');
		}
	}
	printf("\n\n\t? ");
}

#ifndef STANDALONE
gets(buf)
char	*buf;
{
register char *lp;
register c;

	lp = buf;
	for (;;) {
		c = getchar() & 0177;
		if (c>='A' && c<='Z')
			c -= 'A' - 'a';
		if (lp != buf && *(lp-1) == '\\') {
			lp--;
			if (c>='a' && c<='z') {
				c += 'A' - 'a';
				goto store;
			}
			switch ( c) {
			case '(':
				c = '{';
				break;
			case ')':
				c = '}';
				break;
			case '!':
				c = '|';
				break;
			case '^':
				c = '~';
				break;
			case '\'':
				c = '`';
				break;
			}
		}
	store:
		switch(c) {
		case '\n':
		case '\r':
			c = '\n';
			*lp++ = '\0';
			return;
		case '\b':
		case '#':
			lp--;
			if (lp < buf)
				lp = buf;
			continue;
		case '@':
			lp = buf;
			putchar('\n');
			continue;
		default:
			*lp++ = c;
		}
	}
}
#endif
