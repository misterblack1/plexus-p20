FFLAG =

CC = cc

CFLAGS = -O

install:
	-if vax; then cd vax; \
		make -f profiler.mk ARGS="$(ARGS)" CFLAGS="$(CFLAGS)" \
		FFLAG="$(FFLAG)" LDFLAGS="$(LDFLAGS)" install clobber; \
		else cd pdp11; \
		make -f profiler.mk ARGS="$(ARGS)" CFLAGS="$(CFLAGS)" \
		FFLAG="$(FFLAG)" LDFLAGS="$(LDFLAGS)" install clobber; fi

clobber:
