#	MAKE THE WHOLE SYSTEM
#
#	make all -- makes everything from source and build a
#		base file system.  It uses 30000 (1K) blocks on $DEV
#		and 20000 (1K) blocks on $BASEDEV.
#	make proto -- makes just the proto file.
#	make restart "X=file" -- restarts the make with the
#		given library, uts, command, or game but will not copy,
#		clean, make the proto, or make the base.
#	BEFORE A MAKE CHECK THESE PARAMETERS:
FS=/f18
DEV=/dev/dk18
BASEFS=/f17
BASEDEV=/dev/dk17
PROTO=true
COPY=
CLEAN=true
SYSMAKE=true
STRIP=
NOMKFS=
REL=1.0
VER=

proto: 
	PROTO=true COPY= CLEAN= SYSMAKE= STRIP= sh makeall

all: proto system base32

system:
	cmd= lib= uts= games= PROTO= COPY=$(COPY) CLEAN=$(CLEAN) \
		SYSMAKE=$(SYSMAKE) STRIP=$(STRIP) REL=$(REL) VER=$(VER) \
		FS=$(FS) DEV=$(DEV) sh makeall
base32:
	sh makebase $(FS) $(BASEFS) $(BASEDEV) 32
base16:
	sh makebase $(FS) $(BASEFS) $(BASEDEV) 16 $(NOMKFS)
tape:
	sh -x makereltape $(BASEFS) $(BASEDEV)
clean:
	cmd= lib= uts= games= PROTO= COPY= CLEAN=true SYSMAKE= STRIP= \
		REL= VER= FS=$(FS) DEV=$(DEV) sh makeall
restart:
	cmd=$(cmd) lib=$(lib) uts=$(uts) games=$(games) PROTO= \
		COPY= CLEAN= SYSMAKE=$(SYSMAKE) STRIP=$(STRIP) REL=$(REL) \
		VER=$(VER) FS=$(FS) DEV=$(DEV) sh makeall
